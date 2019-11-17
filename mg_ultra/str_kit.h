/*Bunch of useful string stuff*/

#ifndef __STR_KIT__
#define __STR_KIT__

#include <string>
#include <vector>

using namespace std;

/*All methods are in the str_kit namespace*/
namespace str_kit {
	//Returns EXIT about the last operation
	int getErrorCode();

	/*splits string into a vector of strings based on char token*/
	vector<string> splitOnToken(string line, char token);

	/*checks if the string is a digit*/
	bool isADigit(const string& line);

	/*check if the string can be parsed to a signed int*/
	bool isInt(const string& str);

	/*check if the string can be parsed to a float*/
	bool isFloat(const string& str);

	/*Checks if a section of a string is equal to another string, safe to go over
	e.g. "str_kit" with "_k" will be valid at location 3*/
	bool stringCompareAtLocation(const string& base, int location, const string& token);

	/*Takes a float and formats it to score string: XX.XX% or XXX.XX%, if whole XX%*/
	string convertToScoreString(float value, bool whole);
	
	/*Takes a string and replaces all instances token with insert */
	string replaceToken(string str, const string& token, const string& replacement);

	//Takes a vector of inputs, adds them together with a filler, ignore preceding removes the first number of terms
	string reconstituteVectorIntoString(vector<string> input, string filler, int ignorePreceding = 0);

	//Takes a vector of ints adds them together with a filler
	string intVectorToString(vector<int> input, const string& filler);

	//Takes a file position and returns a string of all the content, errors will flip getErrorCode()
	string getContentsOfFile(const string& location);

	//removes all trivial whitespacing from start to end of line
	string trimString(string str);

	//pads a string to the left with required whitespace to reach size
	string padStringLeft(string str, int length);

	//takes a string and a bool ptr, returns int, and sets ptr to true/false and returns 0 on failure, valid with null ptr
	int stringToInt(string& str, bool* valid);

	//takes a string and a bool ptr, returns float, and sets ptr to true/false and returns 0.0 on failure, valid with null ptr
	float stringToFloat(string& str, bool* valid);

	//Computes if a string is trivial, which includes empty string, stings beginning with "//" and strings with only space
	bool isTrivialString(string& str);

	//Safely checks if the start of the string is equal to a token
	//slightly more effective that stringCompareAtLocation, easier to use anyway
	bool compareStart(string a, string b);

	enum LexicalAnalysisResult {
		LAR_idFail = 0,
		LAR_valid,
		LAR_lexLengthFail,
		LAR_lexTypeFail,
		LAR_invalidIDString
	};

	//Lexical analysis, checks string for keyword, then attempts to fit the remaining 
	//parameters with lex str, i: interger, f: float, s: string
	LexicalAnalysisResult lexicalAnalysis(string line, string id, string lex);
	
	//converts string to int, no error checking
	//optionally, set param to a non-negative int to choose a value from a space split vec
	int qStringToInt(string line, int param = -1);

	//converts string to int, no error checking
	//optionally, set param to a non-negative int to choose a value from a space split vec
	float qStringToFloat(string line, int param = -1);

	//Takes a vector of strings and creates a tree
	string createBranchFromVector(vector<string> list);
	
	//Returns a loading bar for a given percentage
	string createPercentageBar(float percentage);

}

#endif
