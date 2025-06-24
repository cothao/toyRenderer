#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include "./input/input.h"

class Camera
{
public:

	Camera(glm::vec3 Position, float Delta);

	const float CameraSpeed = 10.5f;
	float Delta = 0;
	float Yaw = -90.f;
	float Pitch = 0.f;
	float LastX = 400;
	float LastY = 300;
	float XOffSet;
	float YOffSet;
	bool FirstMouse = true;
	bool Keys[1024];
	glm::mat4 View = glm::lookAt(
		glm::vec3(CameraFront.x, 0.f, CameraFront.z),
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(0.f, 1.f, 0.f)
	);

	glm::vec3 CameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 CameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 CameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 Direction = glm::vec3(
		cos(glm::radians(Yaw)) * cos(glm::radians(Pitch)),
		sin(glm::radians(Pitch)),
		sin(glm::radians(Yaw)) * cos(glm::radians(Pitch))
	);

	void MoveCamera(double XPos, double YPos);
	void Update(GLFWwindow* Window, float CurrentDelta, Input::InputTypes Inputs);
	glm::mat4 LookAt(glm::vec3 Position, glm::vec3 Target, glm::vec3 Up);

private:
	void CameraInput(GLFWwindow* Window, int keys[1024]);

};

#endif // !CAMERA_H
