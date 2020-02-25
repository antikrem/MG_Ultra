/*Handles the console entity cached*/
#ifndef __SYSTEM_CONSOLE__
#define __SYSTEM_CONSOLE__

#include "system.h"
#include "input.h"
#include "os_kit.h"

#include "component_console.h"
#include "component_no_bounds_control.h"

class SystemConsole : public System {
	InputState inputState;

	string oldText = "";

public:
	SystemConsole() {
		debugName = "s_console";
		target = SubPoolTarget(
			ETConsole
		);
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
			string text = consoleComponent->getConsoleText();
			if (oldText != text) {
				oldText = text;
				textComponent->setText(text);
			}

			//check if backspace is pressed
			if (inputState.pressEvent["console_backspace"]) {
				consoleComponent->backspace();
			}
			//check if tab has been pressed
			else if (inputState.pressEvent["console_tab"]) {
				consoleComponent->addText("    ");
			}
			//check if paste has been pressed
			else if (inputState.press["console_ctrl"] && inputState.pressEvent["console_paste"]) {
				consoleComponent->addText(os_kit::getClipboard());
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
			g_script::executeScriptUnit(ScriptUnit(SS_commandLine, i));
		}
	}

	void cacheFail(EntityPool* pool) override {
		//create a console entity
		auto newEnt = new Entity(ETConsole);

		newEnt->addComponent(new ComponentPosition(-960, 540, 2));
		newEnt->addComponent(new ComponentConsole());

		auto newComponent = new ComponentText();
		newComponent->setText("Console");
		newComponent->setFont("text_consolas58");
		newComponent->setVisible(false);
		newComponent->setRenderIn3D(false);
		newComponent->setScale(0.75f);
		newEnt->addComponent(newComponent->pullForEntity());
		newEnt->addComponent(new ComponentNoBoundsControl());

		pool->addEnt(newEnt, true);
	}

};

#endif