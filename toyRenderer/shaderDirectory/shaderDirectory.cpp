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

void ShaderDirectory::InitializeShaders(glm::mat4 view, glm::mat4 projection)
{
	ShaderDirectory::GetShader("modelShader").Use();
	ShaderDirectory::GetShader("modelShader").SetInt("texture_diffuse1", 0);
	ShaderDirectory::GetShader("modelShader").SetMat4("view", view);
	ShaderDirectory::GetShader("modelShader").SetMat4("projection", projection);

	ShaderDirectory::GetShader("pbrNoMap").Use();
	ShaderDirectory::GetShader("pbrNoMap").SetMat4("view", view);
	ShaderDirectory::GetShader("pbrNoMap").SetMat4("projection", projection);
	ShaderDirectory::GetShader("pbrNoMap").SetInt("irradianceMap", 0);
	ShaderDirectory::GetShader("pbrNoMap").SetInt("prefilterMap", 1);
	ShaderDirectory::GetShader("pbrNoMap").SetInt("brdfLUT", 2);

	ShaderDirectory::GetShader("pbrMaterial").Use();
	ShaderDirectory::GetShader("pbrMaterial").SetMat4("view", view);
	ShaderDirectory::GetShader("pbrMaterial").SetMat4("projection", projection);
	ShaderDirectory::GetShader("pbrMaterial").SetInt("base_map", 3);
	ShaderDirectory::GetShader("pbrMaterial").SetInt("metallic_map", 4);
	ShaderDirectory::GetShader("pbrMaterial").SetInt("normal_map", 5);
	ShaderDirectory::GetShader("pbrMaterial").SetInt("roughness_map", 6);
	ShaderDirectory::GetShader("pbrMaterial").SetInt("aoMap", 7);
	ShaderDirectory::GetShader("pbrMaterial").SetInt("irradianceMap", 0);
	ShaderDirectory::GetShader("pbrMaterial").SetInt("prefilterMap", 1);
	ShaderDirectory::GetShader("pbrMaterial").SetInt("brdfLUT", 2);

	ShaderDirectory::GetShader("equirectangular").Use();
	ShaderDirectory::GetShader("equirectangular").SetInt("equirectangularMap", 0);
	ShaderDirectory::GetShader("equirectangular").SetMat4("projection", projection);
	ShaderDirectory::GetShader("equirectangular").SetMat4("view", view);

	ShaderDirectory::GetShader("shadow").Use();
	ShaderDirectory::GetShader("shadow").SetInt("irradianceMap", 0);
	ShaderDirectory::GetShader("shadow").SetInt("prefilterMap", 1);
	ShaderDirectory::GetShader("shadow").SetInt("brdfLUT", 2);
	ShaderDirectory::GetShader("shadow").SetInt("base_map", 3);
	ShaderDirectory::GetShader("shadow").SetInt("metallic_map", 4);
	ShaderDirectory::GetShader("shadow").SetInt("normal_map", 5);
	ShaderDirectory::GetShader("shadow").SetInt("roughness_map", 6);
	ShaderDirectory::GetShader("shadow").SetInt("aoMap", 7);
	ShaderDirectory::GetShader("shadow").SetInt("depthMap", 8);
};
