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
	bool isADigit(string line);

	/*Checks if a section of a string is equal to another string, safe to go over
	e.g. "str_kit" with "_k" will be valid at location 3*/
	bool stringCompareAtLocation(string base, int location, string token);

	/*Takes a float and formats it to score string: XX.XX% or XXX.XX%, if whole XX%*/
	string convertToScoreString(float value, bool whole);
	
	/*Takes a string and replaces all instances token with insert */
	string replaceToken(string str, string token, string replacement);

	//Takes a vector of inputs, adds them together with a filler, ignore preceding removes the first number of terms
	string reconstituteVectorIntoString(vector<string> input, string filler, int ignorePreceding = 0);

	//Takes a vector of ints adds them together with a filler
	string intVectorToString(vector<int> input, string filler);

	//Takes a file position and returns a string of all the content, errors will flip getErrorCode()
	string getContentsOfFile(string location);

	//Takes a vector of strings and creates a tree
	string createBranchFromVector(vector<string> list);

}

#endif
