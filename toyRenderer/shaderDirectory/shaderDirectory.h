#ifndef SHADERDIRECTORY_H
#define SHADERDIRECTORY_H
#include <string>
#include <map>
#include "../shader.h"

namespace ShaderDirectory
{

	extern std::map<std::string, Shader> Directory;
	
	void SetShader(std::string shaderName, Shader shader);
	void InitializeShaders(glm::mat4 view, glm::mat4 projection);
	Shader& GetShader(std::string shaderName);

}

#endif