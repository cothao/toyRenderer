#ifndef EDITOR_H
#define EDITOR_H
#include "imgui.h"
#include "glm/glm.hpp"

namespace Editor
{

	namespace ObjectContext
	{
		extern float* roughness;
		extern float* metallic;
		extern glm::vec4 * albedo;
	};

	extern bool show_demo_window;
	extern bool show_another_window;
	extern ImVec4 clear_color;
	extern ImGuiIO* io;
	extern float * roughness;
	void Init();
	void StartFrame();
	void RenderMainMenuBar();
	void Config();
	void Terminate();

	namespace {

		float getMainScale();
	}

}

#endif