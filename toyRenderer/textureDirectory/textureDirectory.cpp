#include "./textureDirectory.h"
#include "../stb_image.h"
#include <iostream>
#include "../renderer/renderer.h"

namespace TextureDirectory
{
	extern std::map<std::string, unsigned int> Directory = {};
}

void TextureDirectory::SetTexture(std::string name, const char * path, bool gamma)
{
	Directory[name] = LoadTexture(path, gamma);
}

void TextureDirectory::SetLookupTexture(std::string name)
{
	Directory[name] = LoadLookupTexture();
}

void TextureDirectory::SetHDRTexture(std::string name, const char* path)
{

	Directory[name] = LoadHDRTexture(path);

}

unsigned int TextureDirectory::GetTexture(std::string name)
{
	return Directory[name];
}

unsigned int TextureDirectory::LoadTexture(const char * path, bool gamma)
{

	unsigned int textureID;

	glGenTextures(1, &textureID);
	
	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	
	if (data)
	{

		GLenum internalFormat;
		GLenum dataFormat;
		if (nrComponents == 1)
		{
			internalFormat = dataFormat = GL_RED;
		}
		else if (nrComponents == 3)
		{
			internalFormat = gamma ? GL_SRGB : GL_RGB;
			dataFormat = GL_RGB;
		}
		else if (nrComponents == 4)
		{

			internalFormat = gamma ? GL_SRGB_ALPHA : GL_RGBA;
			dataFormat = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);

		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);

	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;


}


unsigned int TextureDirectory::LoadHDRTexture(const char* path)
{

	int width, height, nrComponents;

	float* data = stbi_loadf(path, &width, &height, &nrComponents, 0);

	unsigned int hdrTexture;

	if (data)
	{

		glGenTextures(1, &hdrTexture);
		glBindTexture(GL_TEXTURE_2D, hdrTexture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);

	}
	else
	{
		std::cout << "Failed to load HDR Texture" << '\n';
	}

	return hdrTexture;

}

unsigned int TextureDirectory::LoadLookupTexture()
{

	unsigned int LUTTexture;

	glGenTextures(1, &LUTTexture);

	// pre-allocate enough memory for the LUT texture.
	glBindTexture(GL_TEXTURE_2D, LUTTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return LUTTexture;

}

void TextureDirectory::BindPBRTextures(unsigned int irradianceMap, unsigned int prefilterMap)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, TextureDirectory::GetTexture("brdfLUTTexture"));
}

void TextureDirectory::BindMapTextures(unsigned int base, unsigned int metallic, unsigned int normal, unsigned int roughness, unsigned int ao)
{

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, base);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, metallic);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, normal);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, roughness);

	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, ao);

}

void TextureDirectory::BindModelTextureMappings(std::string modelName)
{
	BindMapTextures(
		GetTexture(modelName + "_base_map"),
		GetTexture(modelName + "_metallic_map"),
		GetTexture(modelName + "_normal_map"),
		GetTexture(modelName + "_roughness_map"),
		GetTexture(modelName + "_ao_map")
	);
}

void TextureDirectory::SetTextureFromFile()
{
	const char* texturePath = FileDialog::getFile();

	std::string modelName = strrchr(texturePath, '\\');
	std::cout << modelName << '\n';
	if (!texturePath)
		std::cout << "ERROR | Model not found\n";
	std::cout << texturePath << '\n';

	SetTexture(modelName, texturePath, false);

}

void TextureDirectory::SetHDRTextureFromFile()
{
	const char* texturePath = FileDialog::getFile();

	std::string modelName = strrchr(texturePath, '\\');
	std::cout << modelName << '\n';
	if (!texturePath)
		std::cout << "ERROR | Model not found\n";
	std::cout << texturePath << '\n';

	Directory[modelName] = LoadHDRTexture(texturePath);
	
	Renderer::currentHDRTexture = modelName;

}

void TextureDirectory::Unbind2DMaps()
{
	for (int i = 3; i < 8; i++)
	{
		glActiveTexture(GL_TEXTURE3 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}



void TextureDirectory::SetTextureMaps(std::string modelName, std::string modelPath)
{

	//const char* folderPath = modelPath.c_str();

	//TextureDirectory::SetTexture(modelName + "_base_map",  folderPath + , false);
	//TextureDirectory::SetTexture(modelName + "_metallic_map", folderPath, false);
	//TextureDirectory::SetTexture(modelName + "_normal_map", folderPath, false);
	//TextureDirectory::SetTexture(modelName + "_roughness_map", folderPath, false);
	//TextureDirectory::SetTexture(modelName + "_ao_map", folderPath, false);
}
