#include "modelDirectory.h"
#include <cstring>

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

	const char* modelPath = FileDialog::getFile();

	std::string modelName = strrchr(modelPath, '\\');
	std::cout << modelName << '\n';
	if (!modelPath)
		std::cout << "ERROR | Model not found\n";
	std::cout << modelPath << '\n';

	Directory["model"] = Model(modelPath);

}

Model ModelDirectory::GetModel(std::string modelName)
{
	return Directory[modelName];
}