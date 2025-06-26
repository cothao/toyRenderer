#include "renderer.h"
#include "../objects/objects.h"
#include "../stb_image.h"

namespace Renderer
{
	extern glm::mat4 model = glm::mat4(1.);
	extern glm::mat4 view = glm::mat4(1.);
	extern glm::mat4 projection = glm::mat4(1.);
	extern glm::vec3 cameraPos = glm::vec3(1.);
	extern glm::mat4 MVP = glm::mat4(1.);
}

void Renderer::Init()
{

	glEnable(GL_DEPTH_TEST);

     /*
      *  _  _ ___  ___     _____ _____  _______ _   _ ___ ___ 
      * | || |   \| _ \   |_   _| __\ \/ /_   _| | | | _ \ __|
      * | __ | |) |   /     | | | _| >  <  | | | |_| |   / _| 
      * |_||_|___/|_|_\     |_| |___/_/\_\ |_|  \___/|_|_\___|
      *                                                       
      */

	int width, height, nrComponents;

	float* data = stbi_loadf("../images/photo_studio_loft_hall_4k.hdr", &width, &height, &nrComponents, 0);

	unsigned int hdrTexture;

	if (data)
	{

		glGenTextures(1, &hdrTexture);
		glBindTexture(GL_TEXTURE_2D, hdrTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGB, GL_FLOAT, data);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);

	}
	else
	{
		std::cout << "Failed to load HDR Image\n";
	}

	InitShaders();
	InitModels();
};

void Renderer::SetModelMatrix(glm::mat4 model)
{
	Renderer::model = model;
}

void Renderer::SetViewMatrix(glm::mat4 view)
{
	Renderer::view = view;
}

void Renderer::SetProjectionMatrix(glm::mat4 projection)
{
	Renderer::projection = projection;
}

void Renderer::SetCameraPosition(glm::vec3 cameraPos)
{
	Renderer::cameraPos = cameraPos;
}


void Renderer::ClearBuffers()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::ClearColor(glm::vec4 color)
{
	glClearColor(color.r, color.g, color.b, color.a);
}

// Renders the entire scene. Includes shader updates, transformations, etc.
void Renderer::RenderScene()
{

	ClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.f));
	ClearBuffers();

	ShaderDirectory::GetShader("sphereShader").Use();
	ShaderDirectory::GetShader("sphereShader").SetMat4("model", model);
	ShaderDirectory::GetShader("sphereShader").SetMat4("view", view);
	ShaderDirectory::GetShader("sphereShader").SetMat4("projection", projection);
	ShaderDirectory::GetShader("sphereShader").SetVec3("camPos", cameraPos);

	// Draw the models in the model directory
	//for (auto modelKey = ModelDirectory::Directory.begin(); modelKey != ModelDirectory::Directory.end(); modelKey++)
	//{
	//	DrawModel(modelKey->first, "modelShader");
	//}

	Object::Sphere();

}

// Initializes shaders from ShaderDirectory
void Renderer::InitShaders()
{

	ShaderDirectory::SetShader("modelShader", Shader("./shaders/model.vert", "./shaders/model.frag", nullptr));
	ShaderDirectory::SetShader("sphereShader", Shader("./shaders/sphere_pbr.vert", "./shaders/sphere_pbr.frag", nullptr));

}

void Renderer::InitModels()
{

}

glm::mat4 Renderer::GetModelMatrix()
{
	return Renderer::model;
}

glm::mat4 Renderer::GetViewMatrix()
{
	return Renderer::view;
}

glm::mat4 Renderer::GetProjectionMatrix()
{
	return Renderer::projection;
}

glm::vec3 Renderer::GetCameraPosition()
{
	return Renderer::cameraPos;
}

void Renderer::DrawObject()
{

}

void Renderer::DrawModel(std::string modelName, std::string shaderName)
{
	ModelDirectory::Directory[modelName].Draw(ShaderDirectory::GetShader(shaderName));
}