#include "os_kit.h"

#include <fstream>
#include <windows.h>

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

int os_kit::countNewLinesInFile(const std::string& filePath) {
	int count = 0;
	string line;
	ifstream file(filePath);

	if (!file.good()) {
		return -1;
	}

	while (getline(file, line)) {
		count++;
	}

	return count;
}

//returns a vector of all file names in a folder
vector<string> os_kit::getFilesInFolder(const std::string& filePath) {
	vector<string> locations;
	std::string pattern(filePath);
	pattern += "\\*";

	WIN32_FIND_DATA data;
	HANDLE hFind;
	if ((hFind = FindFirstFile(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE) {
		do {
			string location = string(data.cFileName);
			if (location[0] != '.') {
				locations.push_back(data.cFileName);
			}
		} while (FindNextFile(hFind, &data) != 0);
		FindClose(hFind);
	}
	return locations;
}

//RAII style open clipboad
class ClipboardOpenWrapper {
	bool success = false;
public:
	ClipboardOpenWrapper() {
		success = OpenClipboard(nullptr);
	}

	~ClipboardOpenWrapper() {
		if (success) {
			CloseClipboard();
		}
	}

	bool getSucess() {
		return success;
	}
};

string os_kit::getClipboard() {
	ClipboardOpenWrapper clipboardOpen;
	if (!clipboardOpen.getSucess()) {
		return "";
	}

	 //Get handle of clipboard
	HANDLE hClipBoard = GetClipboardData(CF_TEXT);
	if (!hClipBoard) {
		return "";
	}

	//Lock the handle and get text from clipboad
	char* clipTextPtr = static_cast<char*>(GlobalLock(hClipBoard));
	if (!clipTextPtr) {
		return "";
	}

	string clipboardText(clipTextPtr);

	// Release the lock 
	GlobalUnlock(hClipBoard);

	return clipboardText;
}