#ifndef MODELDIRECTORY_H
#define MODELDIRECTORY_H
#include "../model.h"
#include "../fileDialog/fileDialog.h"
#include <string>
#include <map>

namespace ModelDirectory
{

	extern std::map<std::string, Model> Directory;
	void SetModel(std::string modelName, Model model);
	void SetModelFromFile();
	Model GetModel(std::string modelName);

}

#endif