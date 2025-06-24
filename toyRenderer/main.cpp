#include "./api/api.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <memory>
#include "tinyfiledialogs.h"
#include "./input/input.h"
#include "./editor/editor.h"
#include "./renderer/renderer.h"

void processInput(GLFWwindow* window);
void RenderQuad();

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
float deltaTime = 0.f;
float lastFrame = 0.f;

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), deltaTime);

float fov = 45.f;

int main()
{

    stbi_set_flip_vertically_on_load(true);

    API::Init();
    Input::Init(API::window);
    Editor::Init();
    Renderer::Init();

    while (API::WindowIsOpen)
    {

        API::Update(Input::Inputs);

        float currentFrame = glfwGetTime();

        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        camera.Update(API::window, deltaTime, Input::Inputs);

        Renderer::SetModelMatrix(glm::mat4(1.));
        Renderer::SetViewMatrix(camera.View);
        Renderer::SetProjectionMatrix(glm::perspective(glm::radians(fov), 1600.f / 1000.f, 0.1f, 100.f));

        Renderer::RenderScene();

        Editor::StartFrame();

        glfwSwapBuffers(API::window);
        glfwPollEvents();

    }

    Editor::Terminate();
    glfwTerminate();
	return 0;
}

unsigned int VAO, VBO = 0;
void RenderQuad()
{
    if (VBO == 0)
    {

    float vertices[] = 
    {
        -1., 1., 0., 
        1., 1., 0., 
        -1., -1., 0., 
        -1., -1., 0., 
        1., 1., 0., 
        1., -1., 0., 
    };


    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    }
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);

}
