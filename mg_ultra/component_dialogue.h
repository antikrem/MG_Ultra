/*A Component that stores all upcoming dialogue*/
#ifndef __COMPONENT_DIALOGUE__
#define __COMPONENT_DIALOGUE__

#include <mutex>

#include <deque>
#include <map>

#include "component.h"
#include "constants.h"
#include "scriptable_class.h"

struct DialogueFrame {
	string characterFrame;
	string characterName;
	string dialogue;

	DialogueFrame(const string& characterName, const string& dialogue) {
		this->characterName = characterName;
		this->dialogue = dialogue;
	}

	DialogueFrame() {

	}
};

struct Dialogue {
	bool pauseCycle = false;
	deque<DialogueFrame> dialogues;
};

class ComponentDialogue : public Component {
private:
	mutex lock;
	map<int, Dialogue> dialogueFrames;

public:
	ComponentDialogue() {

	}

	bool isDialogueThisFrame(int cycle) {
		unique_lock<mutex> lck(lock);
		return dialogueFrames.count(cycle);
	}

	Dialogue getDialogue(int cycle) {
		unique_lock<mutex> lck(lock);
		Dialogue temp = dialogueFrames[cycle];
		dialogueFrames.erase(cycle);
		return temp;
	}

	void setDialogue(map<int, Dialogue>& dialogueFrames) {
		unique_lock<mutex> lck(lock);
		this->dialogueFrames = dialogueFrames;
	}
};


#endif