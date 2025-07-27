#ifndef HELPERS_H
#include <string>


#define HELPERS_H

std::string getFolderPath(const std::string& path) {
	const size_t found = path.find_last_of("/\\");
	return (found == std::string::npos) ? path : path.substr(0, found);
}

#endif