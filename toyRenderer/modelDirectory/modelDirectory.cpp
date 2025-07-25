#include "modelDirectory.h"
#include "../textureDirectory/textureDirectory.h"
#include <cstring>
#include "../helpers/helpers.h"

namespace ModelDirectory
{
	std::map<std::string, Model> Directory = {};
}

void ModelDirectory::SetModel(std::string modelName, Model model)
{
	Directory[modelName] = model;
}

void ModelDirectory::SetModelFromFile()
{

	const char* stagedModelPath = FileDialog::getFile();

	std::string modelPath = stagedModelPath;
	std::string modelTexturePath = getFolderPath(modelPath) + "\\Textures\\";

	std::string modelName = strrchr(stagedModelPath, '\\');
	if (!stagedModelPath)
		std::cout << "ERROR | Model not found\n";

	Directory[modelName] = Model(modelPath);

}

Model ModelDirectory::GetModel(std::string modelName)
{
	return Directory[modelName];
}