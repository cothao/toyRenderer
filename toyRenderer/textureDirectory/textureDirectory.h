#ifndef TEXTURE_H
#define TEXTURE_H
#include <map>
#include <string>
#include <glad/glad.h>
#include "../fileDialog/fileDialog.h"

namespace TextureDirectory
{

	extern std::map<std::string, unsigned int> Directory;
	void SetTexture(std::string name, const char* path, bool gamma);
	void SetTextureFromFile();
	void SetTextureMaps(std::string modelName, std::string modelPath);
	void SetLookupTexture(std::string name);
	void SetHDRTexture(std::string name, const char* path);
	unsigned int GetTexture(std::string name);
	unsigned int LoadTexture(const char * path, bool gamma);
	unsigned int LoadHDRTexture(const char * path);
	unsigned int LoadLookupTexture();
	void BindPBRTextures(unsigned int irradianceMap, unsigned int prefilterMap);
	void BindModelTextureMappings(std::string modelName);
	void BindMapTextures(unsigned int base, unsigned int metallic, unsigned int normal, unsigned int roughness, unsigned int ao);
	void SetHDRTextureFromFile();
	void Unbind2DMaps();

}

#endif