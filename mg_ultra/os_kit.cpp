#include "os_kit.h"

#include <fstream>

bool os_kit::fileExists(const std::string& name) {
	ifstream file(name.c_str());
	return file.good();
}

string os_kit::getFileAsString(const std::string& filePath) {
	ifstream in;
	in.open(filePath);
	if (!in) {
		return string("");
	}

	string contents;
	string line;
	while (getline(in, line)) {
		contents += line;
		contents += '\n';
	}
	in.close();

	return contents;
}