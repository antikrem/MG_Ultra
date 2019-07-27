#include "str_kit.h"
#include "constants.h"
#include <iostream>
#include <fstream>
#include <sstream>

int lastOperation = EXIT_SUCCESS;

int str_kit::getErrorCode() {
	return lastOperation;
}

vector<string> str_kit::splitOnToken(string line, char token) {
	if (line.size()) {
		while (line.back() == ' ') {
			line.pop_back();
		}
	}

	int length = (int)line.length();
	vector<string> returnVec;
	int lastToken = -1;

	for (int i = 1; i < length; i++) {
		if (line[i] == token) {
			returnVec.push_back(line.substr(lastToken + 1, i - lastToken - 1));
			lastToken = i;
		}
	}
	returnVec.push_back(line.substr(lastToken + 1));

	return returnVec;
}

bool str_kit::isADigit(string line) {
	for (auto character : line) {
		if ((int)character < 48 || (int)character > 57)
			return false;
	}
	return true;
}

bool str_kit::stringCompareAtLocation(string base, int location, string token) {
	//check basic conditions, token.size() + location <= base.size()
	if (location + token.size() > base.size()) {
		return false;
	}

	for (int i = 0; i < (int)token.size(); i++) {
		if (base[location + i] != token[i]) {
			return false;
		}
	}

	return true;
}

string str_kit::convertToScoreString(float value, bool whole) {
	string score;
	string source = to_string(value);
	string wholeNumber = source.substr(0, source.find('.'));
	if (wholeNumber.length() > 1)
		score.append(wholeNumber);
	else {
		score.append("0");
		score.append(wholeNumber);
	}
	if (whole)
		return score+"%";
	score.append(".");
	score.append( source.substr(source.find('.')+1, 2) );
	return score + "%";
}

string str_kit::replaceToken(string str, string token, string replacement) {
	string rstr = str;

	size_t location = rstr.find(token);

	while (location != string::npos) {
		rstr.replace(location, token.size(), replacement);
		location = rstr.find(token, location + token.size());
	}

	return rstr;
}

string str_kit::reconstituteVectorIntoString(vector<string> input, string filler, int ignorePreceding) {
	vector<string> temp = input;
	if (ignorePreceding) {
		for (int i = 0; i < ignorePreceding; i++) {
			temp.erase(temp.begin());
		}
	}

	string str;
	bool flip = false;
	for (auto i : temp) {
		if not(flip)
			flip = true;
		else
			str.append(filler);

		str.append(i);
	}

	return str;
}

string str_kit::intVectorToString(vector<int> input, string filler) {
	string str;
	bool flip = false;
	for (auto i : input) {
		if not(flip)
			flip = true;
		else
			str.append(filler);

		str.append(to_string(i));
	}

	return str;
}

string str_kit::getContentsOfFile(string location) {
	string str;
	ifstream file(location, std::ios::in);
	if (file.is_open()) {
		stringstream sstr;
		sstr << file.rdbuf();
		str = sstr.str();
		file.close();
	}
	else {
		lastOperation = EXIT_FAILURE;
		return "";
	}
	lastOperation = EXIT_SUCCESS;
	return str;
}

//takes a string and a bool ptr, returns int, and sets ptr to true/false, valid with null ptr
int stringToInt(string& str, bool* valid) {
	int value = 0;
	try {
		value = stoi(str, nullptr, 10);
		if (valid) {
			*valid = true;
		}
		return value;
	} 
	catch(invalid_argument) {
		if (valid) {
			*valid = false;
		}
	}
	return 0;
}

//takes a string and a bool ptr, returns float, and sets ptr to true/false, valid with null ptr
float stringToFloat(string& str, bool* valid) {
	float value = 0;
	try {
		value = stof(str, nullptr);
		if (valid) {
			*valid = true;
		}
		return value;
	}
	catch (invalid_argument) {
		if (valid) {
			*valid = false;
		}
	}
	return 0;
}

string str_kit::createBranchFromVector(vector<string> list) {
	string message = "";
	if not(list.size())
		return message;

	for (unsigned int i = 0; i < list.size(); i++) {
		message += " ";
		message += (i == list.size()-1) ? (char)192 : (char)195;
		message += (char)196;
		message += " ";
		message += list[i];
		message += '\n';
	}
	message.pop_back();
	return message;
}