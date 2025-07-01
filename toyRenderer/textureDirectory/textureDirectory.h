#ifndef TEXTURE_H
#define TEXTURE_H
#include <map>
#include <string>
#include <glad/glad.h>

namespace TextureDirectory
{

	extern std::map<std::string, unsigned int> Directory;
	void SetTexture(std::string name, const char* path, bool gamma);
	unsigned int GetTexture(std::string name);
	unsigned int LoadTexture(const char * path, bool gamma);
}

#endif