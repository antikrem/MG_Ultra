/*Catches input in the main function of a glfw application
Use input class to pull inputs*/
#ifndef __INPUT__
#define __INPUT__

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <map>
#include <atomic>
#include <iostream>

#include "constants.h"

#include "ini_parser.h"

//Allows the querying of an input
//A press means that since the previous update, the button has been pressed
//A pressEvent means that the previous desecretion was not pressed, but the current is
struct InputState {
	map<string, unsigned long long int> keyPressCount;
	map<string, unsigned long long int> keyPressEventCount;

	map<string, bool> press;
	map<string, bool> pressEvent;
};

//handles catching key inputs
class InputMaster {
	//Maps an input-type to a KeyScancode,
	//an input type is a string and a KeyScancode is the corresponding glfw scan code
	map<string, int> keyMap;

	//Stores whether a input type is pressed or not
	map<string, atomic<bool>> keyPress;

	//stores the number of keypresses for a key
	map<string, atomic<unsigned long long int>> keyPressCount;

	//stores the number of key events
	//this is when the previous key update was zero, and the current is one
	map<string, atomic<unsigned long long int>> keyPressEventCount;

	//helper function when adding default binds
	void addDefaultBind(INIParser& ini, string section, string keyName, int keycode);

public:
	//Return 1 if key already exists,
	//otherwise 0
	int addToKeyMap(string inputType, int keyCode);

	//returns answer for a press input query
	//-1 means input is not defined, 0 means not pressed, 1 means pressed
	int queryInputPress(string inputType);

	//update all the key hold states
	void updateKeyPressStates(GLFWwindow* window);

	//clears textmode buffer
	void clearTextModeBuffer();

	//pull text from textmode
	string pullFromTextMode();

	//Prepares an input state
	void seedState(InputState* state);

	//updates the InputState provided
	void updateState(InputState* state);

	InputMaster(GLFWwindow* window);

};

namespace input {
	void setCurrentInputMaster(InputMaster* inputMaster);

	InputMaster* getCurrentInputMaster();

}


#endif