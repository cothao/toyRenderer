#include "shaderDirectory.h"

namespace ShaderDirectory
{
	std::map<std::string, Shader> Directory = {};
}

void ShaderDirectory::SetShader(std::string shaderName, Shader shader)
{
	Directory[shaderName] = shader;
}

Shader& ShaderDirectory::GetShader(std::string shaderName)
{
	return Directory[shaderName];
}