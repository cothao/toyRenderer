#ifndef OBJECTS_H
#define OBJECTS_H

#include "glad/glad.h"
#include "glm/glm.hpp"
#include <vector>

namespace Object
{

	extern unsigned int sphereVAO;
	extern unsigned int cubeVAO;
	extern unsigned int cubeVBO;
	extern unsigned int quadVAO;
	extern unsigned int quadVBO;
	extern unsigned int indexCount;

	void Sphere();
	void Cube();
	void Quad();

}

#endif