#include "./api/api.h"
#include "camera.h"
#include <iostream>

Camera::Camera(glm::vec3 Position, float Delta)
	:CameraPos(Position), Delta(Delta)
{

}

void Camera::MoveCamera(double XPos, double YPos)
{
	if (FirstMouse)
	{
		LastX = XPos;
		LastY = YPos;
		FirstMouse = false;
	}
	XOffSet = XPos - LastX;
	YOffSet = LastY - YPos;

	LastX = XPos;
	LastY = YPos;

	const float Sensitivity = 0.1f;

	XOffSet *= Sensitivity;
	YOffSet *= Sensitivity;

	Yaw += XOffSet;
	Pitch += YOffSet;

	if (Pitch > 89.0f)	Pitch = 89.0f;
	if (Pitch < -89.0f)	Pitch = -89.0f;

	Direction.x = glm::cos(glm::radians(Yaw)) * glm::cos(glm::radians(Pitch));
	Direction.y = glm::sin(glm::radians(Pitch));
	Direction.z = glm::sin(glm::radians(Yaw)) * glm::cos(glm::radians(Pitch));

	CameraFront = glm::normalize(Direction);
}

void Camera::CameraInput(GLFWwindow* Window, int keys[1024])
{

	if (keys[GLFW_KEY_W])
		CameraPos += (CameraSpeed * CameraFront) * Delta;

	if (keys[GLFW_KEY_S])
		CameraPos -= (CameraSpeed * CameraFront) * Delta;

	if (keys[GLFW_KEY_A])
		CameraPos -= (glm::normalize(glm::cross(CameraFront, CameraUp)) * CameraSpeed) * Delta;

	if (keys[GLFW_KEY_D])
		CameraPos += (glm::normalize(glm::cross(CameraFront, CameraUp)) * CameraSpeed) * Delta;

}

// Currently buggy, will work on fixing later (I'm never getting back to it).
glm::mat4 Camera::LookAt(glm::vec3 Position, glm::vec3 Target, glm::vec3 Up)
{

	glm::vec3 CameraDirection = glm::normalize(CameraPos - Target);

	glm::vec3 CameraRight = glm::normalize(glm::cross(Up, CameraDirection));

	glm::mat4 Rotation = glm::mat4(
		glm::vec4(CameraRight, .0),
		glm::vec4(CameraUp, .0),
		glm::vec4(CameraDirection, .0),
		glm::vec4(0.,0.,0.,1.)
	);

	glm::mat4 Translation = glm::mat4(
		glm::vec4(1., 0., 0.,  -CameraPos.x),
		glm::vec4(0., 1., 0., -CameraPos.y),
		glm::vec4(0., 0., 1., -CameraPos.z),
		glm::vec4(0., 0., 0., 1.)
	);

	return Translation * Rotation;

};

void Camera::Update(GLFWwindow * Window, float CurrentDelta, Input::InputTypes Inputs)
{
	if (API::GetCurrentMode() == API::VIEW)
	{
		Delta = CurrentDelta;
		CameraInput(Window, Inputs.keys);
		MoveCamera(Inputs.globalMouseX, Inputs.globalMouseY);
		View = glm::lookAt(CameraPos, CameraPos + CameraFront, CameraUp);

	}

}
