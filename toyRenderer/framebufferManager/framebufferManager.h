#ifndef FRAMEBUFFERMANAGER_H
#define FRAMEBUFFERMANAGER_H
#include <map>
#include <string>

struct Buffer
{
	Buffer() = default;
	Buffer(unsigned int FBO, unsigned int RBO);
	unsigned int FBO;
	unsigned int RBO;
};

namespace FramebufferManager
{

	extern std::map<std::string, Buffer> Directory;
	Buffer GetFBO(std::string name);
	void SetFBO(std::string name);

}

#endif