#ifndef EDITOR_H
#define EDITOR_H
#include "imgui.h"

namespace Editor
{

	extern bool show_demo_window;
	extern bool show_another_window;
	extern ImVec4 clear_color;
	extern ImGuiIO* io;

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