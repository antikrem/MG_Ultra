/*A encapsulation of a file reader,
Can be used to read from a file, pulling out values
Best used when field sizes and format is expected
*/

#ifndef __READER__
#define __READER__

#include <fstream>
#include <cstdio>
#include <string>

/*Use this to read from a file path
One reader per file, slow and inefficient, use outside of system loop
Size of the Reader will be the same as the file

*/
class Reader {
	const string path;

	//contents of the file
	unsigned char* data = nullptr;

	//points to the current chracter being read
	unsigned int pointer = 0;
	//points to last chracter in string + 1, think arrays, use < in for loops
	unsigned int lastPointer = 0;
	int status = EXIT_SUCCESS;

public:
	Reader(const string &location) : path(location) {
		//get filesize
		//todo refactor using fseek
		size_t fileSize;
		ifstream in(path, ifstream::binary | ifstream::ate);
		if (in.is_open()) {
			fileSize = (int)in.tellg();
			in.close();
			status = EXIT_SUCCESS;
		}
		else {
			status = EXIT_FAILURE;
			return;
		}

		//copy to unsigned char
		FILE* file = nullptr;
		int sucess = fopen_s(&file, path.c_str(), "rb"); //todo

		data = (unsigned char*)malloc(fileSize);

		if (fread(data, 1, fileSize, file) != fileSize) {
			status = EXIT_FAILURE;
			return;
		}
		lastPointer = fileSize;
	}

	~Reader() {
		free(data);
	}

	//sets pointer at new location, returns exit code
	int setPointer(const unsigned int &pointer) {
		if (pointer >= lastPointer)
			return EXIT_FAILURE;
		this->pointer = pointer;
		return EXIT_SUCCESS;
	}

	//increments the pointer by n bytes
	//returns exit code
	int incrementPointer(const unsigned int &n) {;
		return setPointer(pointer + n);
	}

	//gets a direct data pointer at pointer
	unsigned char* directDataAccess() {
		return data + pointer;
	}

	//Use to check if next bytes have specifc values
	//return true if next section of bytes is the same as seeker
	//If its true, the pointer will be progressed the length of the string if progress is true
	bool checkSection(const string &search, const bool &progress = true) {
		if (search.size() + pointer > lastPointer)
			return false;

		for (unsigned int i = 0; i < search.size(); i++) {
			if (data[pointer + i] != (unsigned char)search[i])
				return false;
		}
		if (progress)
			pointer += search.size();
		return true;
	}

	//Interprets the next n bytes as an unisigned int
	unsigned int interpretIntBytes(const int &n) {
		if (pointer + n > lastPointer) {
			status = EXIT_FAILURE;
			return 0;
		}

		unsigned int total = 0;
		for (int i = n - 1; i >= 0; i--) {
			//total += ((unsigned int) pow(2, 8 * i)) * data[pointer]; //old way
			total += ((unsigned int)data[pointer]) << (8 * i); //better
			pointer++;
		}

		return total;
	}

	//Interprets the next 4 bytes as an unsigned int
	unsigned int interpretInt() {
		return interpretIntBytes(4);
	}


	//Takes the next n bytes and creates a string
	//Will continue until it reaches a null termination or the end
	//pointer will be advanced n characters
	string getTerminatedString(const int &n) {
		string sub;
		for (unsigned int end = pointer + n; pointer < end; pointer++) {
			if (pointer >= lastPointer) {
				return sub;
			}
			else if (!data[pointer]) {
				pointer = end;
				return sub;
			}
			else {
				sub += (char)data[pointer];
			}
		}
		return sub;
	}

	//returns a data segment of given size
	vector<unsigned char> getDataSegment(const int &n) {
		vector<unsigned char> sub;
		for (unsigned int end = pointer + n; pointer < end && pointer < lastPointer; pointer++) {
			sub.push_back(data[pointer]);
		}
		return sub;
	}

	//takes a position within data and treats it directly as a small endiness int
	unsigned int getDirectInt(const unsigned int &location) {
		if (location > lastPointer) {
			return 0;
		}

		return *(unsigned int*)&(data[location]);
	}

	//Provides status after an operation
	int getStatus() {
		return status;
	}

	//checks if unsigned char is asscii, if not returns dot
	static char uchar2ascii(const unsigned char &a) {
		if (a >= 32 && a <= 127) {
			return a;
		}
		return '.';
	}

	//returns a section of text starting from pointer until end
	string printSection(const int &length) {
		string value;
		for (unsigned int i = pointer; i < pointer + length && i < lastPointer; i++) {
			value.push_back(uchar2ascii(data[i]));
			if ((i - pointer) % 15 == 0) {
				value.push_back('\n');
			}
		}
		value.push_back('\n');
		return value;
	}
};


#endif