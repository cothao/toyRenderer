#ifndef INPUT_H
#define INPUT_H

struct GLFWwindow;

namespace Input
{

	// holds all input types
	extern struct InputTypes 
	{
		float globalMouseX;
		float globalMouseY;
		float globalScrollX;
		float globalScrollY;
		int keys[1024];
	};

	extern GLFWwindow* window;
	extern InputTypes Inputs;

	// Initializes event listening for a range of inputs
	void Init(GLFWwindow*  window);

	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

}

#endif