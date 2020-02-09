#include "input.h"
#include <string>
#include <mutex>

#define KEYBINDS_FILE_LOCATION "settings/key_binds.ini"

string textBuffer;
mutex textBufferLock;

//pointer to current input master
InputMaster* currentPointer = nullptr;

void textModeCallBack(GLFWwindow* window, unsigned int codepoint) {
	lock_guard<mutex> lock(textBufferLock);
	textBuffer.push_back((char)codepoint);
}

void InputMaster::addDefaultBind(INIParser& ini, string section, string keyName, int keycode) {
	addToKeyMap(keyName, ini.get(section, keyName, keycode));
}

int InputMaster::addToKeyMap(string inputType, int keyCode) {
	int startingMaps = keyMap.count(inputType);
	keyMap[inputType] = keyCode;
	keyPress[inputType] = 0;
	keyPressCount[inputType] = 0;
	keyPressEventCount[inputType] = 0;
	return startingMaps;
}

int InputMaster::queryInputPress(string inputType) {
	if not(keyPress.count(inputType)) {
		return -1;
	}
	return keyPress[inputType].load();
}

void InputMaster::updateKeyPressStates(GLFWwindow* window) {
	for (auto i : keyMap) {
		unsigned long long int previousCount = keyPressCount[i.first].load();
		unsigned long long int currentState = (unsigned long long int)(glfwGetKey(window, i.second) == GLFW_PRESS);
		bool previousPress = keyPress[i.first];

		keyPress[i.first].store(currentState);

		keyPressCount[i.first].store(currentState + previousCount);

		keyPressEventCount[i.first].store(keyPressEventCount[i.first].load() + (!previousPress && currentState));
	}
}

void InputMaster::clearTextModeBuffer() {
	lock_guard<mutex> lock(textBufferLock);
	textBuffer.clear();
}

string InputMaster::pullFromTextMode() {
	lock_guard<mutex> lock(textBufferLock);
	string value = textBuffer;
	textBuffer.clear();
	return value;
}

void InputMaster::seedState(InputState* state) {
	for (auto i : keyMap) {
		state->keyPressCount[i.first] = 0;
		state->press[i.first] = 0;
		state->keyPressEventCount[i.first] = 0;
		state->pressEvent[i.first] = 0;
	}
}

void InputMaster::updateState(InputState* state) {
	for (auto i : keyMap) {
		//check state has places
		if not(state->keyPressCount.count(i.first)) {
			state->keyPressCount[i.first] = keyPressCount[i.first];
		}
		if not(state->keyPressEventCount.count(i.first)) {
			state->keyPressEventCount[i.first] = keyPressEventCount[i.first];
		}

		//check press directly, set press event by state check if state is larger
		state->press[i.first] = keyPress[i.first];
		state->pressEvent[i.first] = (state->keyPressEventCount[i.first] != keyPressEventCount[i.first].load());

		state->keyPressCount[i.first] = keyPressCount[i.first];
		state->keyPressEventCount[i.first] = keyPressEventCount[i.first];
	}
}

InputMaster::InputMaster(GLFWwindow* window) {
	INIParser ini(KEYBINDS_FILE_LOCATION);

	addDefaultBind(ini, "builtin", "esc", GLFW_KEY_ESCAPE);

	addDefaultBind(ini, "builtin_movement", "left", GLFW_KEY_LEFT);
	addDefaultBind(ini, "builtin_movement", "right", GLFW_KEY_RIGHT);
	addDefaultBind(ini, "builtin_movement", "up", GLFW_KEY_UP);
	addDefaultBind(ini, "builtin_movement", "down", GLFW_KEY_DOWN);

	addDefaultBind(ini, "builtin_gameplay", "shoot", GLFW_KEY_Z);
	addDefaultBind(ini, "builtin_gameplay", "bomb", GLFW_KEY_X);
	addDefaultBind(ini, "builtin_gameplay", "dash", GLFW_KEY_LEFT_CONTROL);
	addDefaultBind(ini, "builtin_gameplay", "focus", GLFW_KEY_LEFT_SHIFT);

	addDefaultBind(ini, "builtin_console", "console_open", GLFW_KEY_GRAVE_ACCENT);
	addDefaultBind(ini, "builtin_console", "console_enter", GLFW_KEY_ENTER);
	addDefaultBind(ini, "builtin_console", "console_newline", GLFW_KEY_LEFT_SHIFT);
	addDefaultBind(ini, "builtin_console", "console_backspace", GLFW_KEY_BACKSPACE);
	addDefaultBind(ini, "builtin_console", "console_tab", GLFW_KEY_TAB);
	addDefaultBind(ini, "builtin_console", "console_previousline", GLFW_KEY_UP);
	addDefaultBind(ini, "builtin_console", "console_nextline", GLFW_KEY_DOWN);
	addDefaultBind(ini, "builtin_console", "console_ctrl", GLFW_KEY_LEFT_CONTROL);
	addDefaultBind(ini, "builtin_console", "console_paste", GLFW_KEY_V);

	glfwSetCharCallback(window, &textModeCallBack);
}

void input::setCurrentInputMaster(InputMaster* inputMaster) {
	currentPointer = inputMaster;
}

InputMaster* input::getCurrentInputMaster() {
	return currentPointer;
}