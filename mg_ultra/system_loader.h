/*Loading is done asyncronously by the loader
Loader runs on game loop master, and uses the registar to control switching of levels
Loader interfaces with gl handler for mgt loading
Loader also has a list of entities on cycle timer
Loader loads a table from file, The file is in ent_table format

Ent_table:


---ent declaraction

1: spawn condition: Sets the spawn conditions for next entities
@cycle [int] - spawn on cycle
@counter [int] - spawn on counter
@callback [string] - spawn on an emmitted callback
@immediate - spawn now

2: ent declaration
ent [int] - ent direct
ent [string] - ent table look up

3. component adding
+[Component name] ...parameters

4 executing script against component
->{script...}

*/
#ifndef __SYSTEM_LOADER__
#define __SYSTEM_LOADER__

#include "system.h"

#include "registar.h"
#include "os_kit.h"
#include "script_master.h"

#include "component_timer.h"
#include "component_graphics.h"
#include "component_position.h"

enum TargetSpecification {
	TaSp_noTarget,
	TaSp_cycle,
	TaSp_counter,
	TaSp_callback,
	TaSp_immediate,
};

struct TargetFullSpecification {
	TargetSpecification target = TaSp_noTarget;
	int cycle = 0;
	string callback = "";
};

class SystemLoader : public System {
	map<int, shared_ptr<Entity>> cycleEnt;

	//error stuff
	int lineNumber;
	string file;

	static string loadTableComponentNameToScriptName(string loadName) {
		string name = "Component" + loadName;
		name[9] = name[9] - 32;
		return name;
	}

	//transfers ents from 
	void transferBufferedEntsToGame(EntityPool* pool) {
		//get cycle
		int cycle;
		registar->get("cycle", &cycle);
		
		//get ents equal to cycle
		for (auto it = cycleEnt.begin(); it != cycleEnt.end() && it->first < cycle; it = cycleEnt.begin()) {
			pool->addEnt(it->second);
			cycleEnt.erase(it);
		}
	}

	//clears all ents in future queue
	void deleteAllFutureEnts() {
		cycleEnt.clear();
	}

	//converts registar values to next file path, returns empty string on error
	string generateLoadTable() {
		string state;
		registar->get("next_state", &state);

		if (state == "title") {
			//check load sequence is valid
			if not(os_kit::fileExists("sequences\\title\\load_table.txt")) {
				err::logMessage("LOAD: error, failed to load title sequence as the load_table is not found at: sequences\\title\\load_table.txt");
				return "";
			}
			return "sequences\\title\\load_table.txt";
		}
		else if (state == "level") {
			string campaign;
			int level;
			registar->get("next_campaign", &campaign);
			registar->get("next_level", &level);

			string path = "campaigns\\" + campaign + "\\" + to_string(level) + "\\load_table.txt";

			if not(os_kit::fileExists(path)) {
				err::logMessage("LOAD: error, failed to load requested parameters for last load state event, expected " + path);
				return "";
			}
			else {
				return path;
			}
		}

		err::logMessage("LOAD: error, failed to load, requested state parameter invalid: " + state);
		return "";
	}

	//error stuff
	void pushErrorLoading(string line, string id, string lex) {
		string message = "LOAD: Error loading file: " + file + " at line " + to_string(lineNumber) + ". \n--> ";
		message += "Got line: " + line + "";
		message += "Excepted: \"" + id +"\" ";
		for (auto i : lex) {
			if (i == 'i') {
				message += "[int] ";
			} 
			else if (i == 'f') {
				message += "[float] ";
			}
			else if (i == 's') {
				message += "[string] ";
			}
			else {
				message = "LOAD: Error loading file: " + file + " at line " + to_string(lineNumber) + ": invalid lex: " + lex;
				break;
			}
		}
		err::logMessage(message);
	}

	//takes a line and uses it to update the target
	bool updateTarget(string line, TargetFullSpecification* spec) {
		//conduct lexical analysis
		str_kit::LexicalAnalysisResult result;

		if ( result = str_kit::lexicalAnalysis(line, "@cycle", "i") ) {
			if (result == str_kit::LAR_valid) {
				spec->target = TaSp_cycle;
				spec->cycle = str_kit::qStringToInt(line, 1);
				return true;
			}
			else {
				pushErrorLoading(line, "@cycle", "i");
				return false;
			}
		}

		else if (result = str_kit::lexicalAnalysis(line, "@counter", "i")) {
			if (result == str_kit::LAR_valid) {
				spec->target = TaSp_counter;
				spec->cycle = str_kit::qStringToInt(line, 1);
				return true;
			}
			else {
				pushErrorLoading(line, "@counter", "i");
				return false;
			}
		}

		else {
			err::logMessage("LOAD: Error loading file: " + file + " at line " + to_string(lineNumber) + ": unknown id");
			return false;
		}

	}

	//Takes a line and generates a ptr, null indicates the line could not be converted
	shared_ptr<Entity> createEnt(string line) {
		//lex analysis for first template
		if (str_kit::lexicalAnalysis(line, "ent", "i") == str_kit::LAR_valid) {
			return make_shared<Entity>( str_kit::qStringToInt(line, 1) );
		}
		else if (str_kit::lexicalAnalysis(line, "ent", "s") == str_kit::LAR_valid) {
			string token = str_kit::splitOnToken(line, ' ')[1];
			if (stringToEntType(token) < 0) {
				err::logMessage("LOAD: Error resolving token " + token + " to a recognized ent type at line " + to_string(lineNumber) + " in file " +  file);
				return nullptr;
			}
			return make_shared<Entity>( stringToEntType(token) );
		}
		err::logMessage("LOAD: Error creating ent at" + to_string(lineNumber) + " in file " + file +
			+"\n --> Expected: ent [float:ent_type]/[string:ent_type]");
		return nullptr;
	}

	//adds ent to imediate pool
	bool pushEnt(shared_ptr<Entity> ent, TargetFullSpecification target) {
		if (target.target == TaSp_noTarget) {
			err::logMessage("LOAD: Error, Entity pushed at " + to_string(lineNumber) + " in file " + file + " with no target set prior");
			return false;
		}
		else if (target.target == TaSp_cycle) {
			cycleEnt[target.cycle] = ent;
			return true;
		}
		return false;
	}

	//Adds a component to the lates ent
	bool addComponent(string line, shared_ptr<Entity> ent, string* componentName) {
		if not(ent) {
			err::logMessage("LOAD: Error, attempting to add a component with no prior entity declaration " + to_string(lineNumber) + " in file " + file);
			return false;
		}
		
		//conduct lexical analysis
		str_kit::LexicalAnalysisResult result;

		//position
		if (result = str_kit::lexicalAnalysis(line, "+position", "ff")) {
			*componentName = loadTableComponentNameToScriptName("position");
			if (result == str_kit::LAR_valid) {
				auto newComponent = new ComponentPosition(str_kit::qStringToFloat(line, 1), str_kit::qStringToFloat(line, 2), 0);
				ent->addComponent( newComponent->pullForEntity() );
				return true;
			}

			else if (result == str_kit::LAR_lexLengthFail) {
				if (str_kit::lexicalAnalysis(line, "+position", "fff") == str_kit::LAR_valid) {
					auto newComponent = 
						new ComponentPosition(str_kit::qStringToFloat(line, 1), str_kit::qStringToFloat(line, 2), str_kit::qStringToFloat(line, 3));
					ent->addComponent(newComponent->pullForEntity());
					return true;
				}
			}

			//at this point everything has failed, just return
			err::logMessage("LOAD: Error, attempting to add a component with +position prefix" + to_string(lineNumber) + " in file " + file
				+ "\n --> Expected: +Position [float:x] [float:y] [float:z=0]");
			return false;
		}

		//graphics
		else if (result = str_kit::lexicalAnalysis(line, "+graphics", "s")) {
			*componentName = loadTableComponentNameToScriptName("graphics");
			if (result == str_kit::LAR_valid) {
				auto newComponent = new ComponentGraphics(str_kit::splitOnToken(line, ' ')[1]);
				ent->addComponent(newComponent->pullForEntity());
				return true;
			}
			
			else {
				err::logMessage("LOAD: Error, attempting to add a component with +graphics prefix" + to_string(lineNumber) + " in file " + file
					+ "\n --> Expected: +graphics [string:set_name]");
				return false;
			}
		}

		//callback
		else if (result = str_kit::lexicalAnalysis(line, "+timer", "")) {
			*componentName = loadTableComponentNameToScriptName("timer");
			if (result == str_kit::LAR_valid) {
				auto newComponent = new ComponentTimer();
				ent->addComponent(newComponent->pullForEntity());
				return true;
			}

			else {
				err::logMessage("LOAD: Error, attempting to add a component with +timer prefix" + to_string(lineNumber) + " in file " + file
					+ "\n --> Expected: +timer");
				return false;
			}
		}

		err::logMessage("LOAD: Error, Unknown token used with \"+\" at " + to_string(lineNumber) + " in file " + file);
		return false;
	}

	//executes the given command against a component of an ent
	bool inlineExecuteOnComponent(shared_ptr<Entity> ent, string componentName, string line) {
		string source = "this:get_component(" + componentName + "):" + line.substr(2);
		ScriptUnit su(SS_inlineLoader, source);
		su.addDebugData(" in " + file + " at line " + to_string(lineNumber) + " ");
		su.attachEntity(ent);
		sc.reset();
		su.attachSuccessCallback(&sc);
		executeScriptUnit(su);
		return sc.waitForCompletion();
	}


	bool parseLoadTable(string filepath) {
		file = filepath;
		lineNumber = -1;

		TargetFullSpecification currentTarget;
		shared_ptr<Entity> ent = nullptr;
		//The load_table name of the last component used
		string componentName = "";

		string line;
		ifstream table(filepath);
		bool valid = true;
		while (getline(table, line) && valid) {
			lineNumber++;
			line = str_kit::trimString(line);
			if (str_kit::isTrivialString(line)) {
				continue;
			}

			//check if start is a directive
			if (line[0] == '@') {
				valid = updateTarget(line, &currentTarget);
			}
			//else check if it spawns an ent
			else if ( str_kit::compareStart("ent ", line) ) {
				ent = createEnt(line);
				valid = (bool)ent;
				if (valid) {
					valid = pushEnt(ent, currentTarget);
				}
			}
			//else check if an component needs to be added'
			else if (line[0] == '+') {
				valid = addComponent(line, ent, &componentName);
			}
			//else check if a inline component script ahs been requested
			else if (line.size() > 1 && line[0] == '-' && line[1] == '>') {
				valid = inlineExecuteOnComponent(ent, componentName, line);
			}
		}

		return valid;
	}

public:
	SystemLoader() {
		debugName = "s_loader";
		//empty system
		cacheOnly = true;
		cachedTarget = 0;
	}

	void precycle(EntityPool* pool) override {
		//push current cycle of ents
		transferBufferedEntsToGame(pool);

		//check if loading is requested
		bool loading = false;
		registar->get("loading", &loading);
		//if not loading exit
		if not(loading) {
			return;
		}

		/*a load request has been evaluated by system_gamestate_controller, and has to be completed*/
		//delete future ents
		deleteAllFutureEnts();

		//generate path to load table based on registar
		string path = generateLoadTable();
		//empty path is an error
		if (path == "") {
			return;
		}

		if (!parseLoadTable(path)) {
			err::logMessage("LOAD: Fatal Error, load aborted");
		}
		else {
			err::logMessage("LOAD: Successful, starting level at " + path);
			registar->update("cycle", -1);
		}

		registar->update("loading", false);
		registar->update("cycle_progress", true);

	}

};

#endif