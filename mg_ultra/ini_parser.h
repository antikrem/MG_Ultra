/*Allows parsing INI files and regenerating
into a file*/
#ifndef __INI_PARSER__
#define __INI_PARSER__

#include <string>
#include <stdexcept>
#include <fstream>

#include "mmap.h"

#include "os_kit.h"
#include "str_kit.h"

#define INI_DELIMITER '='
#define COMMENT_DELIMITER ','
#define SECTION_NAME_START_DELIMITER '['
#define SECTION_NAME_END_DELIMITER ']'

struct INIParserException : public std::runtime_error {
	INIParserException(std::string error) : std::runtime_error(error) {

	}
};

//Conviniently works by allowing 
class INIParser {
	string filepath;
	
	//serves as a map of section to maps of keys to values
	map<string, map<string, string>> store;

	//Either adds a new section or does nothing if it already exists
	void addSection(string section) {
		if (!store.count(section)) {
			store[section] = map<string, string>();
		}
	}

	void addEntry(const string& section, const string& line) {
		//add a new section
		addSection(section);
		
		//otherwise parse line
		size_t offset;
		if ((offset = line.find(INI_DELIMITER)) != string::npos) {

			store[section][str_kit::trimString(line.substr(0, offset))] 
				= line.substr(offset + 1, line.size() - offset);
		}
	}

	//writes to new ini file
	void writeINI() {
		ofstream iniFile(filepath);

		for (auto& i : store) {
			if (i.first != "") {
				iniFile << "[" << i.first << "]\n";
			}
			for (auto& j : i.second) {
				iniFile << j.first << INI_DELIMITER << j.second <<"\n";
			}
			iniFile << "\n";
		}
	}

	//returns true if section key combo exists
	bool checkSectionValue(string section, string key) {
		return store.count(section) && store[section].count(key);
	}

public:
	INIParser(const string& filepath) {
		this->filepath = filepath;

		vector<string> entries;
		os_kit::getFileAsString(filepath, entries);

		string section = "";

		for (auto& i : entries) {
			str_kit::trimString(i);
			//ignore trivials
			if (str_kit::isTrivialString(i) || i[0] == COMMENT_DELIMITER) {
				continue;
			}
			else if (i[0] == SECTION_NAME_START_DELIMITER
				&& i[i.size() - 1] == SECTION_NAME_END_DELIMITER) {
				section = i.substr(1, i.size() - 2);
			}
			else {
				addEntry(section, i);
			}
		}
	}

	//gets a value as a int, if not possible returns default
	int get(string section, string key, int backup) {
		if (!checkSectionValue(section, key)) {
			addSection(section);
			store[section][key] = to_string(backup);
			return backup;
		}

		if (str_kit::isInt(store[section][key])) {
			return stoi(store[section][key]);
		}
		else {
			store[section][key] = to_string(backup);
			return backup;
		}
	}

	//optional get for no section
	int get(string key, int backup) {
		return get("", key, backup);
	}

	//gets a value as a float, if not possible returns default
	float get(string section, string key, float backup) {
		if (!checkSectionValue(section, key)) {
			addSection(section);
			store[section][key] = to_string(backup);
			return backup;
		}

		if (str_kit::isFloat(store[section][key])) {
			return stof(store[section][key]);
		}
		else {
			store[section][key] = to_string(backup);
			return backup;
		}
	}

	//optional get for no section
	float get(string key, float backup) {
		return get("", key, backup);
	}

	//gets a value as a string, if not possible returns default
	string get(string section, string key, string backup) {
		if (!checkSectionValue(section, key)) {
			addSection(section);
			store[section][key] = backup;
			return backup;
		}

		return store[section][key];
	}

	//optional get for no section
	string get(string key, string backup) {
		return get("", key, backup);
	}

	//templated sets value into store
	template<class T>
	void set(string section, string key, T value) {
		addSection(section);
		store[section][key] = to_string(value);
	}

	//templated set specialised for string
	template<>
	void set(string section, string key, string value) {
		addSection(section);
		store[section][key] = value;
	}

	//returns all the keys for a given section
	vector<string> getKeys(string section) {
		if (!store.count(section)) {
			return vector<string>();
		}

		vector<string> keys;
		for (auto& i : store[section]) {
			keys.push_back(i.first);
		}

		return keys;
	}

	~INIParser() {
		writeINI();
	}

};

#endif