/*Bunch of useful os stuff*/

#ifndef __OS_KIT__
#define __OS_KIT__

#include <string>
#include <vector>

using namespace std;

/*All methods are in the str_kit namespace*/
namespace os_kit {
	//checks if file exists
	bool fileExists(const std::string& name);

	//loads all the contents of a file from a script
	//empty string on faliure
	string getFileAsString(const std::string& filePath);

	//similar to getFileAsString, but sets each line
	//as a new element in string
	void getFileAsString(const std::string& filePath, vector<string>& results);

	//counts number of new lines within a file
	//returns -1 if file does not exist
	int countNewLinesInFile(const std::string& filePath);

	//returns a vector of all file names in a folder
	vector<string> getFilesInFolder(const std::string& filePath);

	//returns what evers currently in the clipboard
	//returns empty string on error
	string getClipboard();
}

#endif