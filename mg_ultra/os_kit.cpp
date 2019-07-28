#include "os_kit.h"

bool os_kit::fileExists(const std::string& name) {
	ifstream file(name.c_str());
	return file.good();
}