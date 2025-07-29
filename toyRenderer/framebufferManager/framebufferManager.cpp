#include "framebufferManager.h"
#include "glad/glad.h"

Buffer::Buffer(unsigned int FBO, unsigned int RBO)
	:FBO(FBO), RBO(RBO)
{}

namespace FramebufferManager
{
	extern std::map<std::string, Buffer> Directory = {};
}

Buffer FramebufferManager::GetFBO(std::string name)
{
	return Directory[name];
}

void FramebufferManager::SetDepthMapFBO(unsigned int depthMap)
{

	if (Directory.find("depthMap") != Directory.end())
	{
		return; // FBO already exists
	}

	unsigned int FBO;
	unsigned int RBO;
	glGenFramebuffers(1, &FBO);
	glGenRenderbuffers(1, &RBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	Directory["depthMap"] = Buffer(FBO, RBO);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void FramebufferManager::SetFBO(std::string name)
{
	if (Directory.find(name) != Directory.end())
	{
		return; // FBO already exists
	}

	unsigned int FBO;
	unsigned int RBO;
	glGenFramebuffers(1, &FBO);
	glGenRenderbuffers(1, &RBO);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);
	Directory[name] = Buffer(FBO, RBO);
}

void FramebufferManager::GenerateCubemapFromEnvironmentMap(Shader shader, unsigned int texture, unsigned int environmentMap, glm::mat4 projection,glm::mat4 views[])
{

	// convert HDR equirectangular environment map to cubemap equivalent
	shader.Use();
	shader.SetInt("equirectangularMap", 0);
	shader.SetMat4("projection", projection);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.

	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferManager::GetFBO("capture").FBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		shader.SetMat4("view", views[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, environmentMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Object::Cube(); // renders a 1x1 cube
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, environmentMap);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

}

void FramebufferManager::GenerateCubemapFromIrradianceMap(Shader shader, unsigned int irradianceMap, unsigned int environmentMap, glm::mat4 projection, glm::mat4 views[])
{

	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferManager::GetFBO("capture").FBO);
	glBindRenderbuffer(GL_RENDERBUFFER, FramebufferManager::GetFBO("capture").RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

	// irradianceshader
	shader.Use();
	shader.SetInt("environmentMap", 0);
	shader.SetMat4("projection", projection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, environmentMap);

	glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferManager::GetFBO("capture").FBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		shader.SetMat4("view", views[i]);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Object::Cube();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FramebufferManager::GenerateCubemapFromPrefilterMap(Shader shader, unsigned int prefilterMap, unsigned int environmentMap, glm::mat4 projection, glm::mat4 views[])
{
	shader.Use();
	shader.SetInt("environmentMap", 0);
	shader.SetMat4("projection", projection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, environmentMap);

	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferManager::GetFBO("capture").FBO);
	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		// reisze framebuffer according to mip-level size.
		unsigned int mipWidth = 128 * std::pow(0.5, mip);
		unsigned int mipHeight = 128 * std::pow(0.5, mip);
		glBindRenderbuffer(GL_RENDERBUFFER, FramebufferManager::GetFBO("capture").RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		shader.SetFloat("roughness", roughness);

		for (unsigned int i = 0; i < 6; ++i)
		{
			shader.SetMat4("view", views[i]);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			Object::Cube();
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void FramebufferManager::GenerateTextureFromFramebuffer(Shader shader, unsigned int lookUpTexture)
{

	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferManager::GetFBO("capture").FBO);
	glBindRenderbuffer(GL_RENDERBUFFER, FramebufferManager::GetFBO("capture").RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lookUpTexture, 0);

	glViewport(0, 0, 512, 512);
	shader.Use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Object::Quad();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}
