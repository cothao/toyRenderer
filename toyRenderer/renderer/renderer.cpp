#include "renderer.h"
#include "../objects/objects.h"

namespace Renderer
{
	extern glm::mat4 model = glm::mat4(1.);
	extern glm::mat4 view = glm::mat4(1.);
	extern glm::mat4 projection = glm::mat4(1.);
	extern glm::mat4 MVP = glm::mat4(1.);
}

void Renderer::Init()
{

	glEnable(GL_DEPTH_TEST);
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

	ShaderDirectory::GetShader("modelShader").Use();
	ShaderDirectory::GetShader("modelShader").SetMat4("model", model);
	ShaderDirectory::GetShader("modelShader").SetMat4("view", view);
	ShaderDirectory::GetShader("modelShader").SetMat4("projection", projection);

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

void Renderer::DrawObject()
{

}

void Renderer::DrawModel(std::string modelName, std::string shaderName)
{
	ModelDirectory::Directory[modelName].Draw(ShaderDirectory::GetShader(shaderName));
}