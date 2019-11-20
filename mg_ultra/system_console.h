/*Handles the console entity cached*/
#ifndef __SYSTEM_CONSOLE__
#define __SYSTEM_CONSOLE__

#include "system.h"
#include "input.h"

#include "component_console.h"

class SystemConsole : public System {
	InputState inputState;

public:
	SystemConsole() {
		debugName = "s_console";
		cachedTarget = ETConsole;
		cacheOnly = true;
		input::getCurrentInputMaster()->seedState(&inputState);
	}

	void cacheHandle(shared_ptr<Entity> ent) override {
		input::getCurrentInputMaster()->updateState(&inputState);
		
		auto consoleComponent = ent->getComponent<ComponentConsole>();
		auto textComponent = ent->getComponent<ComponentText>();

		//update console text view based on console input
		if (inputState.pressEvent["console_open"]) {
			//console is set to active
			if (consoleComponent->flipFlag()) {
				input::getCurrentInputMaster()->clearTextModeBuffer();
				textComponent->setVisible(true);
			} 
			else {
				textComponent->setVisible(false);
			}
		}

		//handle if text is active
		if (consoleComponent->getActive()) {
			//add error stuff to console component
			consoleComponent->addPreviousLines(err::getConsoleMessage());

			//put text into console buffer
			consoleComponent->addText(input::getCurrentInputMaster()->pullFromTextMode());
			textComponent->setText(consoleComponent->getConsoleText());

			//check if backspace is pressed
			if (inputState.pressEvent["console_backspace"]) {
				consoleComponent->backspace();
			}
			//check if tab has been pressed
			else if (inputState.pressEvent["console_tab"]) {
				consoleComponent->addText("    ");
			}
			//check if previous line has been request
			else if (inputState.pressEvent["console_previousline"]) {
				consoleComponent->recallAStep();
			}
			else if (inputState.pressEvent["console_nextline"]) {
				consoleComponent->redownAStep();
			}
			//check if enter has been entered
			else if (inputState.pressEvent["console_enter"]) {
				//check if console_newline is pressed
				if (input::getCurrentInputMaster()->queryInputPress("console_newline")) {
					consoleComponent->addNewLine();
				}
				else {
					consoleComponent->pushCurrentLine();
				}
			}
		}

		//execute current command line buffer
		vector<string> buffer = consoleComponent->pullCommandLineBuffer();

		for (auto& i : buffer) {
			//create a scriptunit and send to be execute
			executeScriptUnit(ScriptUnit(SS_commandLine, i));
		}
	}

	void cacheFail(EntityPool* pool) override {
		//create a console entity
		auto newEnt = new Entity(ETConsole);
		auto newComponent = new ComponentPosition(-940, 540, 2);
		newEnt->addComponent(newComponent->pullForEntity());
		auto newComponent1 = new ComponentConsole();
		newEnt->addComponent(newComponent1->pullForEntity());
		auto newComponent2 = new ComponentText();
		newComponent2->setText("Console");
		newComponent2->setFont("text_consolas58");
		newComponent2->setVisible(false);
		newComponent2->setScale(0.5f);
		newComponent2->setRenderIn3D(false);
		newEnt->addComponent(newComponent2->pullForEntity());
		pool->addEnt(newEnt, true);
	}

};

#endif