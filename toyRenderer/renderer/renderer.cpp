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
	extern int nrRows = 7;
	extern int nrColumns = 7;
	extern float spacing = 2.5;
	extern float metallic = .5f;
	extern float roughness = .5f;

	extern glm::vec3 lightPositions[] = {
	glm::vec3(-10.0f,  10.0f, 10.0f),
	glm::vec3(10.0f,  10.0f, 10.0f),
	glm::vec3(-10.0f, -10.0f, 10.0f),
	glm::vec3(10.0f, -10.0f, 10.0f),
	};

	extern glm::vec3 lightColors[] = {
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f)
	};
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

     /*
      *  _    ___   _   ___      _____ _____  _______ _   _ ___ ___ ___ 
      * | |  / _ \ /_\ |   \    |_   _| __\ \/ /_   _| | | | _ \ __/ __|
      * | |_| (_) / _ \| |) |     | | | _| >  <  | | | |_| |   / _|\__ \
      * |____\___/_/ \_\___/      |_| |___/_/\_\ |_|  \___/|_|_\___|___/
      *                                                                 
      */

	TextureDirectory::SetTexture("rusted_metal_base_map", "../images/rusted_metal/rustediron2_basecolor.png", true);
	TextureDirectory::SetTexture("rusted_metal_metallic_map", "../images/rusted_metal/rustediron2_metallic.png", true);
	TextureDirectory::SetTexture("rusted_metal_normal_map", "../images/rusted_metal/rustediron2_normal.png", true);
	TextureDirectory::SetTexture("rusted_metal_roughness_map", "../images/rusted_metal/rustediron2_roughness.png", true);

	InitShaders();
	InitModels();

	ShaderDirectory::GetShader("pbrNoMap").Use();
	ShaderDirectory::GetShader("pbrNoMap").SetMat4("view", view);
	ShaderDirectory::GetShader("pbrNoMap").SetMat4("projection", projection);

	ShaderDirectory::GetShader("sphereShader").Use();
	ShaderDirectory::GetShader("sphereShader").SetMat4("view", view);
	ShaderDirectory::GetShader("sphereShader").SetMat4("projection", projection);
	ShaderDirectory::GetShader("sphereShader").SetFloat("ao", 1.f);
	ShaderDirectory::GetShader("sphereShader").SetInt("base_map", 0);
	ShaderDirectory::GetShader("sphereShader").SetInt("metallic_map", 1);
	ShaderDirectory::GetShader("sphereShader").SetInt("normal_map", 2);
	ShaderDirectory::GetShader("sphereShader").SetInt("roughness_map", 3);

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

	ClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.f));
	ClearBuffers();

	ShaderDirectory::GetShader("sphereShader").Use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureDirectory::GetTexture("rusted_metal_base_map"));

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TextureDirectory::GetTexture("rusted_metal_metallic_map"));

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, TextureDirectory::GetTexture("rusted_metal_normal_map"));

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, TextureDirectory::GetTexture("rusted_metal_roughness_map"));

	ShaderDirectory::GetShader("sphereShader").SetVec3("camPos", Renderer::GetCameraPosition());

	for (int i = 0; i < 4; i++)
	{

		ShaderDirectory::GetShader("sphereShader").SetVec3("lightPosition[" + std::to_string(i) + ']', lightPositions[i]);
		ShaderDirectory::GetShader("sphereShader").SetVec3("lightColor[" + std::to_string(i) + ']', lightColors[i]);

	}

	ShaderDirectory::GetShader("sphereShader").SetMat4("view", view);

	ShaderDirectory::GetShader("sphereShader").SetMat4("model", model);
	//ShaderDirectory::GetShader("sphereShader").SetFloat("roughness", glm::clamp(roughness, 0.05f, 1.0f));
	//ShaderDirectory::GetShader("sphereShader").SetFloat("metallic", metallic);

	Object::Sphere();

	for (int i = 0; i < 4; i++)
	{
	
		glm::mat4 model = glm::mat4(1.);
		model = glm::translate(model, lightPositions[i]);

		SetModelMatrix(model);
		ShaderDirectory::GetShader("lightShader").Use();
		ShaderDirectory::GetShader("lightShader").SetMat4("model", model);
		ShaderDirectory::GetShader("lightShader").SetMat4("view", view);
		ShaderDirectory::GetShader("lightShader").SetMat4("projection", projection);
		Object::Sphere();
	
	}

	ShaderDirectory::GetShader("pbrNoMap").Use();

	glm::mat4 stageModel = glm::mat4(1.);

	stageModel = glm::translate(stageModel, glm::vec3(4., 0., 0.));

	SetModelMatrix(stageModel);

	ShaderDirectory::GetShader("pbrNoMap").SetVec3("camPos", Renderer::GetCameraPosition());
	ShaderDirectory::GetShader("pbrNoMap").SetVec3("albedo", glm::vec3(1., 0., 0.));

	for (int i = 0; i < 4; i++)
	{

		ShaderDirectory::GetShader("pbrNoMap").SetVec3("lightPosition[" + std::to_string(i) + ']', lightPositions[i]);
		ShaderDirectory::GetShader("pbrNoMap").SetVec3("lightColor[" + std::to_string(i) + ']', lightColors[i]);

	}

	ShaderDirectory::GetShader("pbrNoMap").SetMat4("view", view);

	ShaderDirectory::GetShader("pbrNoMap").SetMat4("model", model);

	ShaderDirectory::GetShader("pbrNoMap").SetFloat("metallic", 0.5);
	ShaderDirectory::GetShader("pbrNoMap").SetFloat("roughness", 0.5);
	ShaderDirectory::GetShader("pbrNoMap").SetFloat("ao", 1.);

	Object::Sphere();

	// Draw the models in the model directory
	//for (auto modelKey = ModelDirectory::Directory.begin(); modelKey != ModelDirectory::Directory.end(); modelKey++)
	//{
	//	DrawModel(modelKey->first, "modelShader");
	//}


}

// Initializes shaders from ShaderDirectory
void Renderer::InitShaders()
{

	ShaderDirectory::SetShader("modelShader", Shader("./shaders/model.vert", "./shaders/model.frag", nullptr));
	ShaderDirectory::SetShader("sphereShader", Shader("./shaders/sphere_pbr.vert", "./shaders/sphere_pbr.frag", nullptr));
	ShaderDirectory::SetShader("lightShader", Shader("./shaders/sphere_pbr.vert", "./shaders/light.frag", nullptr));
	ShaderDirectory::SetShader("pbrNoMap", Shader("./shaders/pbr_no_map.vert", "./shaders/pbr_no_map.frag", nullptr));

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