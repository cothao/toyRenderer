#include "modelDirectory.h"
#include "../textureDirectory/textureDirectory.h"
#include <cstring>
#include "../helpers/helpers.h"

namespace ModelDirectory
{
	extern glm::vec3 size = glm::vec3(1.);
	extern glm::vec3 rotation = glm::vec3(1.);
	extern glm::vec3 translation = glm::vec3(1.);
	std::map<std::string, Model> Directory = {};
}

void ModelDirectory::SetModel(std::string modelName, Model model)
{
	Directory[modelName] = model;
}

void ModelDirectory::SetModelFromFile()
{

	const char* stagedModelPath = FileDialog::getFile();



	if (!stagedModelPath)
	{
		std::cout << "ERROR | Model not found\n";
	}
	else
	{
		std::string modelPath = stagedModelPath;
		std::string modelName = strrchr(stagedModelPath, '\\');
		Directory[modelName] = Model(modelPath);
	}

}

Model ModelDirectory::GetModel(std::string modelName)
{
	return Directory[modelName];
}