#include "str_kit.h"
#include "constants.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

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

bool str_kit::isADigit(const string& line) {
	for (auto character : line) {
		if ((int)character < 48 || (int)character > 57)
			return false;
	}
	return true;
}

bool str_kit::isInt(const string& str) {
	try {
		stoi(str);
	}
	catch (invalid_argument e) {
		return false;
	}
	return true;
}

bool str_kit::isFloat(const string& str) {
	try {
		stof(str);
	}
	catch (invalid_argument e) {
		return false;
	}
	return true;
}

bool str_kit::stringCompareAtLocation(const string& base, int location, const string& token) {
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
		return score + "%";
	score.append(".");
	score.append(source.substr(source.find('.') + 1, 2));
	return score + "%";
}

string str_kit::replaceToken(string str, const string& token, const string& replacement) {
	size_t location = str.find(token);

	while (location != string::npos) {
		str.replace(location, token.size(), replacement);
		location = str.find(token, location + token.size());
	}

	return str;
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

string str_kit::intVectorToString(vector<int> input, const string& filler) {
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

string str_kit::getContentsOfFile(const string& location) {
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

string str_kit::trimString(string str) {
	//trim start
	while (str.size() && (
		str[0] == ' ' ||
		str[0] == '\t' ||
		str[0] == '\n' ||
		str[0] == '\v' ||
		str[0] == '\f' ||
		str[0] == '\r'
		)
		) {
		str.erase(0, 1);
	}
	//trim end
	while (str.size() && (
		str.back() == ' ' ||
		str.back() == '\t' ||
		str.back() == '\n' ||
		str.back() == '\v' ||
		str.back() == '\f' ||
		str.back() == '\r'
		)
		) {
		str.pop_back();
	}
	return str;
}

string str_kit::padStringLeft(string str, int length) {
	length = length - (int)str.size();
	length = length < 0 ? 0 : length;

	return string(" ", length) + str;
}

//takes a string and a bool ptr, returns int, and sets ptr to true/false, valid with null ptr
int str_kit::stringToInt(string& str, bool* valid) {
	int value = 0;
	try {
		value = stoi(str, nullptr, 10);
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

//takes a string and a bool ptr, returns float, and sets ptr to true/false, valid with null ptr
float str_kit::stringToFloat(string& str, bool* valid) {
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

bool str_kit::isTrivialString(string& str) {
	if not(str.size()) {
		return true;
	}
	//check if its a commented line
	else if (str.size() >= 2 && str[0] == '/' && str[1] == '/') {
		return true;
	}
	//check for lines with non-important characters only
	if (str.find_first_not_of(" \t\n\v\f\r") == std::string::npos) {
		return true;
	}
	return false;
}

bool str_kit::compareStart(string a, string b) {
	size_t size = min(a.size(), b.size());
	for (size_t i = 0; i < size; i++) {
		if (a[i] != b[i]) {
			return false;
		}
	}
	return true;
}

str_kit::LexicalAnalysisResult str_kit::lexicalAnalysis(string line, string id, string lex) {
	if (!compareStart(line, id)) {
		return LAR_idFail;
	}
	vector<string> vec = splitOnToken(line, ' ');

	//check if the number of parameters is correct
	if (lex.find('%') == string::npos && vec.size() != lex.size() + 1) {
		return LAR_lexLengthFail;
	}

	//used for idChar perma set
	bool permaSet = false;
	char lastChar = -1;

	//check if lex is fine
	for (unsigned int i = 1; i < vec.size(); i++) {
		char idChar;

		// idChar depends on if permaSet is active
		if (permaSet) {
			idChar = lastChar;
		}
		else {
			idChar = lex[i - 1];
		}

		lastChar = idChar;
		if (idChar == 'i') {
			if (!isInt(vec[i])) {
				return LAR_lexTypeFail;
			}
		}
		else if (idChar == 'f') {
			if (!isFloat(vec[i])) {
				return LAR_lexTypeFail;
			}
		}
		else if (idChar == 's') {
			pass;
		}
		else if (idChar == '%') {
			permaSet = true;
		}
		else {
			return LAR_invalidIDString;
		}
	}

	return LAR_valid;
}

int str_kit::qStringToInt(string line, int param) {
	string str;
	if (param < 0) {
		str = line;
	}
	else {
		vector<string> vec = splitOnToken(line, ' ');
		str = vec[param];
	}
	return stoi(str);;
}

float str_kit::qStringToFloat(string line, int param) {
	string str;
	if (param < 0) {
		str = line;
	}
	else {
		vector<string> vec = splitOnToken(line, ' ');
		str = vec[param];
	}
	return stof(str);;
}

string str_kit::createBranchFromVector(vector<string> list) {
	string message = "";
	if not(list.size())
		return message;

	for (unsigned int i = 0; i < list.size(); i++) {
		message += " ";
		message += (i == list.size() - 1) ? (char)192 : (char)195;
		message += (char)196;
		message += " ";
		message += list[i];
		message += '\n';
	}
	message.pop_back();
	return message;
}

string str_kit::createPercentageBar(float fraction) {
	//clamp percentage
	fraction = fraction < 0 ? 0 : 100 * fraction;
	fraction = fraction > 100 ? (float)100 : fraction;


	string str = padStringLeft(to_string((int)round(fraction)), 3) + "%: [";
	fraction = round(fraction / 10);

	str += string((int)fraction, '#');
	str += string(10 - (int)fraction, '.');
	str += "]";

	return str;
}