/*
Handles dialogue
*/
#ifndef __SYSTEM_DIALOGUE__
#define __SYSTEM_DIALOGUE__

#include "system.h"

#include "registar.h"

#include "component_position.h"
#include "component_text.h"
#include "component_dialogue.h"

#define DIALOGUE_NEXT_CHAR_CD 5
#define DIALOGUE_NEXT_FRAME_NP 100
#define DIALOGUE_NEXT_FRAME_P 1000


class SystemDialogue : public System {
	InputState inputState;

	bool inDialogue = false;
	//list of frames for this cycle
	Dialogue currentDialogue;
	//the current frame being handled
	DialogueFrame currentFrame;

	//current dialogue being displayed
	string dialogue;
	//current character stuff
	string characterFrame;
	string characterName;

	//cooldown for next dialogue update
	int cooldown = 0;

public:
	SystemDialogue() {
		debugName = "s_dialogue";
		target = SubPoolTarget(
			ETDialogue
		);

		input::getCurrentInputMaster()->seedState(&inputState);
	}

	void cacheHandle(shared_ptr<Entity> ent) override {
		auto compText = ent->getComponent<ComponentText>();
		input::getCurrentInputMaster()->updateState(&inputState);

		//if in dialogue, need to update
		//when in dialogue, currentFrame is the current dialogue frame being rendered
		if (inDialogue) {

			//update dialogue string
			//check if current dialogue is corrent size 
			if (dialogue.size() < currentFrame.dialogue.size()) {
				//append a single character if enough cooldown has passed
				cooldown++;
				if (cooldown > DIALOGUE_NEXT_CHAR_CD) {
					dialogue += currentFrame.dialogue[dialogue.size()];
					compText->setText(currentFrame .characterName + ": " + dialogue);
					cooldown = 0;
				}
			}
			//after a frame is fully drawn, need to pause 
			//for paused, until update
			//for unpaused, enough time
			else if (currentDialogue.pauseCycle && (cooldown < DIALOGUE_NEXT_FRAME_P)) {
				if (inputState.pressEvent["shoot"]) {
					cooldown += DIALOGUE_NEXT_FRAME_P;
				}
				else {
					cooldown++;
				}

			}
			else if (!currentDialogue.pauseCycle && cooldown < DIALOGUE_NEXT_FRAME_NP) {
				cooldown++;
			}
			else {
				//clear current buffer
				dialogue.clear();
				cooldown = 0;

				//check if theres a dialogue frame coming in
				if (currentDialogue.dialogues.size()) {
					currentFrame = currentDialogue.dialogues[0];
					currentDialogue.dialogues.pop_front();

					characterFrame = currentFrame.characterFrame;
					characterName = currentFrame.characterName;
				}
				//otherwise dialogue is over
				else {
					currentFrame.dialogue = "";
					inDialogue = false;
					compText->setVisible(false);
					registar->update("in_dialogue", false);
					registar->update("in_dialogue_pause", false);
				}
			}
		}

		//otherwise just keep checking for new frames
		else {
			int cycle = 0;
			registar->get("cycle", &cycle);
			bool loadDialogue = false;
			registar->get("load_dialogue", &loadDialogue);

			auto compDia = ent->getComponent<ComponentDialogue>();
			if (!loadDialogue && compDia->isDialogueThisFrame(cycle)) {
				currentDialogue = compDia->getDialogue(cycle);
				inDialogue = true;
				registar->update("in_dialogue", true);
				registar->update("in_dialogue_pause", currentDialogue.pauseCycle);
				compText->setText("");
				compText->setVisible(true);
				cooldown = DIALOGUE_NEXT_FRAME_P;
			}
		}

	}

	void cacheFail(EntityPool* pool) override {
		//create a dialogue textbox
		auto newEnt = new Entity(ETDialogue);

		newEnt->addComponent(new ComponentPosition(-930, -500, 2));

		auto newComponent = new ComponentText();
		newComponent->setText("Console");
		newComponent->setFont("text_consolas58");
		newComponent->setVisible(false);
		newComponent->setRenderIn3D(false);
		newComponent->setScale(0.60f);
		newEnt->addComponent(newComponent->pullForEntity());
		newEnt->addComponent(new ComponentDialogue());

		pool->addEnt(newEnt, true);
	}

};

#endif