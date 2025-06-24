#ifndef API_H
#define API_H

#include <glad/glad.h>
#include "../stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include "../shader.h"
#include "../camera.h"
#include "../model.h"
#include "../input/input.h"

struct GLFWwindow;


/*Handles all interfaces with the OpenGL Api*/
namespace API
{

	enum MODE
	{
		VIEW,
		EDIT
	};

	extern GLFWwindow* window;
	extern MODE CURRENT_MODE;

	/*Intializes window context*/
	void Init();

	// Updates in render pass
	void Update(Input::InputTypes Input);

	void HandleKeyStrokes(int keys[1024]);

	/* Check to see if window is open.
	Returns boolean value*/
	bool WindowIsOpen();

	void SetViewPortSize(int width, int height);

	MODE GetCurrentMode();

	GLFWmonitor * getPrimaryMonitorPointer();

}

#endif