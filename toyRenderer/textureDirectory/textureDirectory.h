#ifndef TEXTURE_H
#define TEXTURE_H
#include <map>
#include <string>
#include <glad/glad.h>
#include "../fileDialog/fileDialog.h"

class TextureMapping
{
	public:
		TextureMapping(std::string name = "", unsigned int id = 0, void (*textureMapper)(std::string) = nullptr);
		//~TextureMapping();
		std::string name = "";
		unsigned int id = 0;
		void (*SetTextureMapping)(std::string) = nullptr;

};

namespace TextureDirectory
{

	extern std::map<std::string, unsigned int> Directory;
	extern std::map<std::string, TextureMapping> TextureMappings;
	void SetTexture(std::string name, const char* path, bool gamma);
	void SetTextureFromFile();
	void SetTextureMapping(std::string textureType);
	void SetLookupTexture(std::string name);
	void SetHDRTexture(std::string name, const char* path);
	unsigned int GetTexture(std::string name);
	unsigned int GetTextureMapping(std::string name);
	unsigned int LoadTexture(const char * path, bool gamma);
	unsigned int LoadHDRTexture(const char * path);
	unsigned int LoadLookupTexture();
	void LoadDepthTexture();
	void BindPBRTextures(unsigned int irradianceMap, unsigned int prefilterMap);
	void BindModelTextureMappings();
	void BindMapTextures(unsigned int base, unsigned int metallic, unsigned int normal, unsigned int roughness, unsigned int ao);
	void SetHDRTextureFromFile();
	void Unbind2DMaps();

}

#endif