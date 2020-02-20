/*
Handles dialogue loading
*/
#ifndef __SYSTEM_DIALOGUE_LOADER__
#define __SYSTEM_DIALOGUE_LOADER__

#include <iostream>
#include <fstream>

#include "system.h"

#include "os_kit.h"
#include "str_kit.h"

#include "component_dialogue.h"

#define DIALOGUE_FILENAME "dialogue.txt" 

class SystemDialogueLoader : public System {
	//creates a string to the folder of target to load
	//will return empty string if its a non level 
	string getLoadFolderPath() {
		string state;
		registar->get("next_state", &state);

		string path;
		if (state == "level") {
			string campaign;
			int level;
			registar->get("next_campaign", &campaign);
			registar->get("next_level", &level);

			path = "campaigns\\" + campaign + "\\" + to_string(level) + "\\";
		}
		else {
			return "";
		}

		//compose string into a path to file
		if not(os_kit::fileExists(path + DIALOGUE_FILENAME)) {
			return "";
		}
		else {
			return path + DIALOGUE_FILENAME;
		}
	}

	//populates dialogue frame map
	void populateDialogueFrames(map<int, Dialogue>& dialogueFrames, const string& filePath) {
		std::ifstream file(filePath);

		string line;
		
		int cycle = -1;
		Dialogue factoryDialogue;
		string characterName;

		while (getline(file, line)) {
			line = str_kit::trimString(line);
			vector<string> args = str_kit::splitOnToken(line, ' ');

			if (str_kit::isTrivialString(line)) {
				pass;
			}
			else if (line[0] == '@') {
				//push previous
				if (cycle >= 0) {
					dialogueFrames[cycle] = factoryDialogue;
				}
				
				factoryDialogue.dialogues.clear();

				cycle = str_kit::qStringToInt(args[0].substr(1));
				factoryDialogue.pauseCycle = (args.size() >= 2 && args[1] == "pause");
				
			}
			else if (line[0] == '%') {
				//Set dialogue frame data
				characterName = args[0].substr(1);
			}
			else {
				factoryDialogue.dialogues.push_back(
					DialogueFrame(characterName, line)
				);
			}
		}

		//make sure last factoryDialogue will be pushed
		if (cycle >= 0) {
			dialogueFrames[cycle] = factoryDialogue;
		}
	}

public:
	SystemDialogueLoader() {
		debugName = "s_dialogue_loader";
		target = SubPoolTarget(
			ETDialogue
		);
	}

	void cacheHandle(shared_ptr<Entity> ent) override {
		//check dialogue flag
		bool loadDialogue = false;
		registar->get("load_dialogue", &loadDialogue);

		if (!loadDialogue) {
			return;
		}

		string loadPath = getLoadFolderPath();
		
		if (!loadPath.size()) {
			return;
		}

		map<int, Dialogue> dialogueFrames;

		populateDialogueFrames(dialogueFrames, loadPath);

		ent->getComponent<ComponentDialogue>()->setDialogue(dialogueFrames);
		registar->update("load_dialogue", false);
	}

};

#endif