#include "input.h"
#include <GLFW/glfw3.h>

namespace Input 
{
    GLFWwindow* window = nullptr;
    InputTypes Inputs;
}

void Input::Init(GLFWwindow* window)
{
	Input::window = window;
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

}

void Input::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
};

void Input::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    Input::Inputs.globalMouseX = xpos;
    Input::Inputs.globalMouseY = ypos;
};

void Input::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{

    Input::Inputs.globalScrollX = xoffset;
    Input::Inputs.globalScrollY = yoffset;

};

void Input::key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    
    if (key >= 0 && key <= 1024)
    {
        if (action == GLFW_PRESS)
        {
            Input::Inputs.keys[key] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            Input::Inputs.keys[key] = false;
        }
    }

};
