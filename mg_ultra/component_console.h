/*Has methods for updating a local buffer
and keeping a list of all commands
*/
#ifndef __COMPONENT_CONSOLE__
#define __COMPONENT_CONSOLE__

#include "component.h"
#include "input.h"

#include <vector>
#include <string>

class ComponentConsole : public Component {
	bool active = false;
	//previous lines, like from log
	vector<string> previousLines;
	//previous commmands
	vector<string> previousCommands;
	//commands to be send to script system for execution
	vector<string> commandBuffer;
	string currentLine;
	mutex lock;

	//number of lines displayed above the current line
	atomic<int> numberOfPreviousLines = 10;
	//skips this number of lines from the bottom, scrolling up to look at previous text
	atomic<int> skip = 0;

	//the current previous line recall pointer, current line is how far to go
	atomic<int> previousRecall = -1;
public:
	ComponentConsole() {
		previousCommands.push_back("[TOP]");
	}

	//flips active flag and returns current state
	bool flipFlag() {
		lock_guard<mutex> lck(lock);
		active = !active;
		return active;
	}

	//get current flag
	bool getActive() {
		lock_guard<mutex> lck(lock);
		return active;
	}

	//adds to the console current line buffer
	void addText(string text) {
		lock_guard<mutex> lck(lock);
		currentLine.append(text);
	}

	//adds new line to the line buffer, press enter
	void addNewLine() {
		lock_guard<mutex> lck(lock);
		previousRecall = -1;
		currentLine.push_back('\n');
	}

	//removes last character from current line buffer
	void backspace() {
		lock_guard<mutex> lck(lock);
		previousRecall = -1;
		if (currentLine.size()) {
			currentLine.pop_back();
		}
	}

	//pushes current line into line buffer
	void pushCurrentLine() {
		lock_guard<mutex> lck(lock);
		previousRecall = -1;
		if (currentLine.size()) {
			previousCommands.push_back(currentLine);
			commandBuffer.push_back(currentLine);
			err::logMessage("<<" + currentLine);
			currentLine.clear();
		}
	}

	//adds to previous line buffer
	void addPreviousLines(vector<string> buffer) {
		lock_guard<mutex> lck(lock);
		if (buffer.size()) {
			previousLines.insert(previousLines.end(), buffer.begin(), buffer.end());
		}
	}

	//goes up a step in previous command recall, and replaces current buffer with recall
	void recallAStep() {
		lock_guard<mutex> lck(lock);
		//check if the previous line recall can handle more presses
		if (previousRecall < (int)previousCommands.size()-1) {
			previousRecall++;
		}
		currentLine.clear();
		currentLine = previousCommands[(int)previousCommands.size() - previousRecall - 1];
	}
	
	//goes down a step
	void redownAStep() {
		lock_guard<mutex> lck(lock);
		//check if the previous line recall can handle more presses
		if (previousRecall > 0) {
			previousRecall--;
			currentLine.clear();
			currentLine = previousCommands[(int)previousCommands.size() - previousRecall - 1];
		}
		else {
			previousRecall = -1;
			currentLine.clear();
		}
	}

	//returns current command buffer, and clears
	vector<string> pullCommandLineBuffer() {
		lock_guard<mutex> lck(lock);
		vector<string> buffer = commandBuffer;
		commandBuffer.clear();
		return buffer;
	}

	//gets the console text, the text that needs to be shown
	string getConsoleText() {
		vector<string> pastLines;
		lock_guard<mutex> lck(lock);
		//need to create numberOfPreviousLines
		//implemented by scrolling from end of past commands
		//until enough has beenn generated for both numberOfPreviousLines + skip
		for (int i = (int)previousLines.size()-1; i >= 0; i--) {
			vector<string> splitLine = str_kit::splitOnToken(previousLines[i], '\n');
			pastLines.insert(pastLines.begin(), splitLine.begin(), splitLine.end());
			if ((int)pastLines.size() > (numberOfPreviousLines + skip)) {
				break;
			}
		}

		//Most likely theres too many lines, these need to be trimmed
		//Alternativly theres not enough lines, pad for 5 by adding more
		while ((int)pastLines.size() > numberOfPreviousLines) {
			pastLines.erase(pastLines.begin());
		}

		//Alternativly theres not enough lines, pad for more by adding more
		while ((int)pastLines.size() < numberOfPreviousLines) {
			pastLines.insert(pastLines.begin(), "");
		}

		string buffer;
		for (auto i : pastLines) {
			buffer.append(i);
			buffer.append("\n");
		}

		return buffer + " :" + str_kit::replaceToken(currentLine, "\n", "\n :");
	}

};


#endif __COMPONENT_CONSOLE__