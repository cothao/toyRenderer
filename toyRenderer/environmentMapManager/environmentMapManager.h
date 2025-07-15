#ifndef ENVIRONMENTMAPMANAGER_H
#define ENVIRONMENTMAPMANAGER_H

namespace EnvironmentMapManager
{

	extern unsigned int envCubemap;
	extern unsigned int irradianceMap;
	extern unsigned int prefilterMap;

	void SetEnvCubeMap();
	void SetIrradianceMap();
	void SetPrefilterMap();
	unsigned int GetEnvCubeMap();
	unsigned int GetIrradianceMap();
	unsigned int GetPrefilterMap();
	unsigned int * GetEnvCubeMapPtr();
	unsigned int * GetPrefilterMapPtr();
	unsigned int * GetIrradianceMapPtr();

}

#endif