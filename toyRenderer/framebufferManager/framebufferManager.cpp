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

void FramebufferManager::SetFBO(std::string name)
{
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
