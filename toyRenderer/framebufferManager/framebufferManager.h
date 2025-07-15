#ifndef FRAMEBUFFERMANAGER_H
#define FRAMEBUFFERMANAGER_H
#include <map>
#include <string>
#include "../shader.h"
#include "../objects/objects.h"
#include "glm/glm.hpp"

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
	void GenerateCubemapFromEnvironmentMap(Shader shader, unsigned int texture, unsigned int environmentMap, glm::mat4 projection, glm::mat4 views[]);
	void GenerateCubemapFromIrradianceMap(Shader shader, unsigned int irradianceMap, unsigned int environmentMap, glm::mat4 projection, glm::mat4 views[]);
	void GenerateCubemapFromPrefilterMap(Shader shader, unsigned int prefilterMap, unsigned int environmentMap, glm::mat4 projection, glm::mat4 views[]);
	void GenerateTextureFromFramebuffer(Shader shader, unsigned int lookUpTexture);

}

#endif