#include "api.h"
#include <GLFW/glfw3.h>

static void glfw_error_callback(int error, const char* description);

namespace API
{
    GLFWwindow* window = nullptr;
    MODE CURRENT_MODE = EDIT;
}

void API::Init()
{
    glfwSetErrorCallback(glfw_error_callback);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    API::window = glfwCreateWindow(1600, 1000, "ToyRenderer", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

}

void API::Update(Input::InputTypes Input)
{

    API::HandleKeyStrokes(Input.keys);

}

bool API::WindowIsOpen()
{
    return !glfwWindowShouldClose(API::window);
}

GLFWmonitor* API::getPrimaryMonitorPointer()
{
    return glfwGetPrimaryMonitor();
};

void API::SetViewPortSize(int width, int height)
{

    glViewport(0,0,width, height);

};

void API::HandleKeyStrokes(int keys[1024])
{

    if (keys[GLFW_KEY_E])
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        CURRENT_MODE = EDIT;
    }
    if (keys[GLFW_KEY_V])
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        CURRENT_MODE = VIEW;
    }

};

API::MODE API::GetCurrentMode()
{

    return CURRENT_MODE;

};

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

