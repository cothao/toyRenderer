#ifndef RENDERER_H
#define RENDERER_H
#include "glm/glm.hpp"
#include "glad/glad.h"
#include "../shaderDirectory/shaderDirectory.h"
#include "../modelDirectory/modelDirectory.h"
#include "../camera.h"

namespace Renderer
{

	extern glm::mat4 model;
	extern glm::mat4 view;
	extern glm::mat4 projection;
	extern glm::vec3 cameraPos;
	extern int nrRows;
	extern int nrColumns;
	extern float spacing;
	extern glm::vec3 lightPositions[];
	extern glm::vec3 lightColors[];

	// Initializes models, shaders, buffers, matrices, etc.
	void Init();
	void SetModelMatrix(glm::mat4 model);
	void SetViewMatrix(glm::mat4 view);
	void SetProjectionMatrix(glm::mat4 projection);
	void SetCameraPosition(glm::vec3 cameraPos);
	void ClearBuffers();
	void ClearColor(glm::vec4 color);
	void RenderScene();
	void InitShaders();
	void InitModels();
	void ConfigureShaders();
	void DrawObject();
	void DrawModel(std::string modelName, std::string shaderName);

	glm::mat4 GetModelMatrix();
	glm::mat4 GetViewMatrix();
	glm::mat4 GetProjectionMatrix();
	glm::vec3 GetCameraPosition();



}

#endif