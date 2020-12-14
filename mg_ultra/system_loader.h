/** 
 * Loading is done asyncronously by the loader
 * Loader runs on game loop master, and uses the registar to control switching of levels
 * Loader interfaces with gl handler for mgt loading
 * Loader also has a list of entities on cycle timer
 * Loader loads a table from file, The file is in ent_table format

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
#include "vec_kit.h"
#include "script_master.h"
#include "mmap.h"
#include "algorithm_ex.h"

#include "component_timer.h"
#include "component_graphics.h"
#include "component_position.h"

#define REPLACEMENT_TOKEN "%"

//token for prelevel script
#define PRELEVL_SCRIPT "prelevel.lua" 

enum TargetSpecification {
	TaSp_noTarget,
	TaSp_cycle,
	TaSp_counter,
	TaSp_callback,
	TaSp_immediate,
};

struct TargetFullSpecification {
	TargetSpecification target = TaSp_noTarget;
	vector<int> cycles;
	string callback = "";
};

class SystemLoader : public System {
	MMap<int, shared_ptr<Entity>> cycleEnts;
	map<int, string> cycleStrings;
	string immediateScript = "";

	map<string, string> templates;

	//error stuff
	int lineNumber;
	string file;

	// Static generator for spawning entities from entity spawn line
	std::function<shared_ptr<Entity>(string)> entityGeneratorLine = [](string line) {
		return make_shared<Entity>(str_kit::qStringToInt(line, 1));
	};

	// Static generator for spawning entities from entity string name
	std::function<shared_ptr<Entity>(string)> entityGeneratorToken = [](string token) {
		return make_shared<Entity>(stringToEntType(token));
	};

	static string loadTableComponentNameToScriptName(string loadName) {
		loadName = loadName.substr(1);
		string name = "Component" + loadName;
		name[9] = name[9] - 32;
		return name;
	}

	//transfers ents from cycle ents to game
	void transferBufferedEntsToGame(EntityPool* pool, int cycle) {
		// Check registar for player entity spawning flag
		bool spawnEnts = true;
		registar->get("cycle_spawn_ents", &spawnEnts);

		//get ents equal to cycle
		for (auto it = cycleEnts.begin(); it != cycleEnts.end() && it->first < cycle; it = cycleEnts.begin()) {
			auto ents = cycleEnts.peel(it->first);
			if (spawnEnts) {
				for (auto i : ents) {
					pool->addEnt(i);
				}
			}
		}

		if (!spawnEnts) {
			registar->update("cycle_spawn_ents", true);
		}
	}

	//executes a script
	//returns sucess
	bool executeScript(const string& script, bool isPreLevel = false) {
		ScriptUnit su(SS_inlineLoader, script);
		if (!isPreLevel) {
			su.addDebugData(" in " + file + " at line " + to_string(lineNumber) + " ");
		}
		else {
			su.addDebugData(" in level preload");
		}
		
		sc.reset();
		su.attachSuccessCallback(&sc);
		g_script::executeScriptUnit(su);
		return sc.waitForCompletion();
	}

	//execute cycle scipts to game
	void executeBufferedCycleScripts(int cycle) {
		for (auto it = cycleStrings.begin(); 
				it != cycleStrings.end() && it->first < cycle; 
				it = cycleStrings.begin()) {
			auto& cycleScript = it->second;
			executeScript(cycleScript);
			cycleStrings.erase(it);
		}
	}

	//clears all ents in future queue
	void deleteAllFutureEnts() {
		cycleEnts.clear();
	}

	//clears all scripts in future queue
	void deleteAllFutureScripts() {
		cycleStrings.clear();
	}

	//creates a string to the folder of target to load
	string getLoadFolderPath() {
		string state;
		registar->get("next_state", &state);

		if (state == "title") {
			return "campaigns\\title\\";
		}
		else if (state == "level") {
			string campaign;
			int level;
			registar->get("next_campaign", &campaign);
			registar->get("next_level", &level);

			string path = "campaigns\\" + campaign + "\\" + to_string(level) + "\\";
			return path;
		}

		err::logMessage("LOAD: error, failed to load, requested state parameter invalid: " + state);
		return "";
	}

	//returns a string to load table in given load folder, empty on error
	string getLoadTablePath(const string& loadFolder) {
		if not(os_kit::fileExists(loadFolder + "load_table.txt")) {
			err::logMessage(
				"LOAD: error, failed to load requested parameters for last load state event, expected " + loadFolder + "load_table.txt"
			);
			return "";
		}
		else {
			return loadFolder + "load_table.txt";
		}
	}

	//returns a string to template table, empty on error
	string getTemplateTablePath(const string& loadFolder) {
		if not(os_kit::fileExists(loadFolder + "template_table.txt")) {
			return "";
		}
		else {
			return loadFolder + "template_table.txt";
		}

	}

	//returns a string to pre level script
	string getPrelevelScript(const string& loadFolder) {
		if not(os_kit::fileExists(loadFolder + PRELEVL_SCRIPT)) {
			return "";
		}
		else {
			return loadFolder + PRELEVL_SCRIPT;
		}
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

		if ( result = str_kit::lexicalAnalysis(line, "@cycle", "i%") ) {
			if (result == str_kit::LAR_valid) {
				spec->target = TaSp_cycle;
				spec->cycles.clear();
				for (unsigned int i = 1; i < str_kit::splitOnToken(line, ' ').size(); i++) {
					spec->cycles.push_back(str_kit::qStringToInt(line, i));
				}
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
				spec->cycles.clear();
				spec->cycles.push_back(str_kit::qStringToInt(line, 1));
				return true;
			}
			else {
				pushErrorLoading(line, "@counter", "i");
				return false;
			}
		}

		else if (result = str_kit::lexicalAnalysis(line, "@immediate", "")) {
			if (result == str_kit::LAR_valid) {
				spec->target = TaSp_immediate;
				return true;
			}
			else {
				pushErrorLoading(line, "@immediate", "");
				return false;
			}
		}

		else {
			err::logMessage("LOAD: Error loading file: " + file + " at line " + to_string(lineNumber) + ": unknown id");
			return false;
		}

	}

	//Takes a line and generates a ptr, empty vector indicates an error
	vector<shared_ptr<Entity>> createEnt(string line, TargetFullSpecification spec) {
		//lex analysis for first template
		if (str_kit::lexicalAnalysis(line, "ent", "i") == str_kit::LAR_valid) {
			return vec_kit::generate(
				(int)spec.cycles.size(),
				SystemLoader::entityGeneratorLine,
				line
			);
		}
		else if (str_kit::lexicalAnalysis(line, "ent", "s") == str_kit::LAR_valid) {
			string token = str_kit::splitOnToken(line, ' ')[1];
			if (stringToEntType(token) < 0) {
				err::logMessage("LOAD: Error resolving token " + token + " to a recognized ent type at line " + to_string(lineNumber) + " in file " +  file);
				return vector<shared_ptr<Entity>>();
			}
			return vec_kit::generate(
				(int)spec.cycles.size(),
				SystemLoader::entityGeneratorToken,
				token
			);
		}
		err::logMessage("LOAD: Error creating ent at" + to_string(lineNumber) + " in file " + file +
			+"\n --> Expected: ent [float:ent_type]/[string:ent_type]");
		return vector<shared_ptr<Entity>>();
	}

	//adds ent to imediate pool
	bool pushEnts(vector<shared_ptr<Entity>> ents, TargetFullSpecification target) {
		if (target.target == TaSp_noTarget) {
			err::logMessage("LOAD: Error, Entity pushed at " + to_string(lineNumber) + " in file " + file + " with no target set prior");
			return false;
		}
		else if (target.target == TaSp_cycle) {
			for (unsigned int i = 0; i < target.cycles.size(); i++) {
				cycleEnts.add(target.cycles[i], ents[i]);
			}
			return true;
		}
		return false;
	}

	//splits a component directive into a script master directive
	//will return empty string on error
	string evaluateComponentLine(string line, string& componentName) {
		line = str_kit::trimString(line);
		//need to find first space
		auto it = line.find(' ');

		//if reached the end use string end
		if (it == std::string::npos) {
			it = line.size();
		}

		//compute script name of component
		componentName = loadTableComponentNameToScriptName(line.substr(0, it));

		//convert parameter list into parameters for create
		string scriptCall = line.substr(it);
		scriptCall = str_kit::trimString(scriptCall);

		//takes a string reference and a starting index
		//scans the token, if the token cannnot be casted to an int or float
		//it will be converted to an int
		int scanLocation = 0;
		while (scanLocation != string::npos) {
			int end = scriptCall.find(' ', scanLocation );
			if (end == string::npos) {
				//set for end of string
				end = scriptCall.size();
			}
			int length = end - scanLocation;

			string sub = scriptCall.substr(scanLocation, length);
			if (!str_kit::isInt(sub) && !str_kit::isFloat(sub) && str_kit::trimString(sub).size()) {
				scriptCall.insert(scanLocation, "\"");
				length ++;
				scriptCall.insert(scanLocation + length, "\"");
				length++;
			}

			//find new scan location
			scanLocation = scriptCall.find(' ', scanLocation + length) + 1;
			//if we couldn't find a space, we get 0
			scanLocation = scanLocation ? scanLocation : string::npos;
		}

		
		replace(scriptCall, ' ', ',');
		scriptCall = "this:add_component(" + componentName +".create(" + scriptCall + "))";

		return scriptCall;
	}

	//Adds a component to the lates ent
	bool addComponent(string line, vector<shared_ptr<Entity>>& ents, string& componentName) {
		if not(ents.size()) {
			err::logMessage("LOAD: Error, attempting to add a component with no prior entity declaration " + to_string(lineNumber) + " in file " + file);
			return false;
		}
		
		//get call
		string call = evaluateComponentLine(line, componentName);
		for (auto& e : ents) {
			ScriptUnit su(SS_inlineLoader, call);
			su.addDebugData(" in " + file + " at line " + to_string(lineNumber) + " ");
			su.attachEntity(e);
			sc.reset();
			su.attachSuccessCallback(&sc);
			g_script::executeScriptUnit(su);
			if (!sc.waitForCompletion()) {
				return false;
			}

		}
		return true;
	}

	//executes the given command against a component of an ent
	bool inlineExecuteOnComponent(vector<shared_ptr<Entity>>& ents, string componentName, string line) {
		for (auto& e : ents) {
			string source = "this:get_component(" + componentName + "):" + line.substr(2);
			ScriptUnit su(SS_inlineLoader, source);
			su.addDebugData(" in " + file + " at line " + to_string(lineNumber) + " ");
			su.attachEntity(e);
			sc.reset();
			su.attachSuccessCallback(&sc);
			g_script::executeScriptUnit(su);
			if (!sc.waitForCompletion()) {
				return false;
			}
		}
		return true;
		
	}

	//Adds a line of scripting to level manifest
	bool loadScriptLevel(string line, const TargetFullSpecification& destination) {
		line = line.substr(2);
		if (destination.target == TaSp_cycle) {
			for (unsigned int i = 0; i < destination.cycles.size(); i++) {
				//attach script
				if (!cycleStrings.count(destination.cycles[i])) {
					cycleStrings[destination.cycles[i]] = "";
				}
				cycleStrings[destination.cycles[i]].append(line + "\n");
			}
			
		}
		else if (destination.target == TaSp_immediate) {
			//attach script
			immediateScript.append(line + "\n");
		}

		return true;
	}

	//evaluates a template
	bool evaluateTemplate(string line, vector<string>& buffer) {
		line = str_kit::trimString(line.substr(1));
		vector<string> templateTokens = str_kit::splitOnToken(line, ' ');

		if (!templates.count(templateTokens[0])) {
			err::logMessage("LOAD: Error evaluating template at" + to_string(lineNumber) + " in file " + file +
				+"\n --> Template name of " + templateTokens[0] + " was not found");
			return false;
		}
		else {
			string templateContents = templates[templateTokens[0]];

			for (int i = 1; i < (int)templateTokens.size(); i++) {
				templateContents = str_kit::replaceToken(
					templateContents, 
					REPLACEMENT_TOKEN + to_string(i),
					templateTokens[i]
				);
			}

			vector<string> temporary = str_kit::splitOnToken(templateContents, '\n');

			extend(buffer, temporary);

			return true;
		}
	}

	//gets the current buffer and load file
	//returns true if theres stuff to process
	bool nextLine(ifstream& table, vector<string>& buffer, string& line) {
		string currentLine;

		if (buffer.size()) {
			//move first member
			currentLine = buffer[0];
			buffer.erase(buffer.begin());

			line.assign(currentLine);
			return true;
		}
		else if (getline(table, currentLine)) {
			lineNumber++;
			line.assign(currentLine);
			return true;
		}

		return false;
	}

	bool parseLoadTable(string filepath) {
		file = filepath;
		lineNumber = -1;
		immediateScript = "";

		TargetFullSpecification currentTarget;
		vector<shared_ptr<Entity>> ents = vector<shared_ptr<Entity>>();
		//The load_table name of the last component used
		string componentName = "";

		string line;
		vector<string> buffer;
		ifstream table(filepath);
		bool valid = true;
		while (valid && nextLine(table, buffer, line)) {

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
				ents = createEnt(line, currentTarget);
				valid = (bool)ents.size();
				if (valid) {
					valid = pushEnts(ents, currentTarget);
				}
			}
			//else check if an component needs to be added'
			else if (line[0] == '+') {
				valid = addComponent(line, ents, componentName);
			}
			//else check if a inline component script has been requested
			else if (line.size() > 1 && line[0] == '-' && line[1] == '>') {
				valid = inlineExecuteOnComponent(ents, componentName, line);
			}
			//else check if a fixed script is added
			else if (line.size() > 1 && line[0] == '<' && line[1] == '<') {
				valid = loadScriptLevel(line, currentTarget);
			}
			//else check for templtes
			else if (line[0] == '#') {
				valid = evaluateTemplate(line, buffer);
			}
		}

		return valid;
	}

	//pushes a template to the current list
	void addTemplate(const string& templateName, const string& templateBody) {
		if (templateName.size()) {
			templates[templateName] = templateBody;
		}
	}

	//for use in parsing template files
	void parseTemplateFile(string filepath) {
		lineNumber = -1;

		string line;
		ifstream table(filepath);

		string lastTemplateName = "";
		string templateText = "";

		while (getline(table, line)) {
			lineNumber++;
			line = str_kit::trimString(line);

			//check for new template
			if (str_kit::isTrivialString(line)) {
				continue;
			}
			else if (line[0] == '[' && line[line.size() - 1] == ']') {
				addTemplate(lastTemplateName, templateText);
				lastTemplateName = line.substr(1, line.size() - 2);
				templateText = "";
			}
			else {
				templateText.append(line + "\n");
			}
		}

		addTemplate(lastTemplateName, templateText);
	}

	//run the state change script
	bool runStateChangeScript() {
		string path = PATH_TO_AUTO_SCRIPT;
		path.append(INIT_STATE_CHANGE);
		string script;

		vector<string> scripts = os_kit::getFilesInFolder(path);
		for (auto& scriptLocation : scripts) {
			script.append(os_kit::getFileAsString(path + scriptLocation));
			script.append("\n");
		}

		ScriptUnit su(SS_inlineLoader, script);
		su.addDebugData(" in state_change autorun");

		sc.reset();
		su.attachSuccessCallback(&sc);
		g_script::executeScriptUnit(su);
		return sc.waitForCompletion();
	}

	//updates level flags
	void updateFlagsOnSuccessfulLoad() {
		string state = "none";
		string campaign = "none";
		int level = 0;

		registar->get("next_state", &state);
		registar->get("next_campaign", &campaign);
		registar->get("next_level", &level);

		registar->update("state", state);
		registar->update("campaign", campaign);
		registar->update("level", level);

		
	}



public:
	SystemLoader() {
		debugName = "s_loader";
		//empty system
		target = SubPoolTarget();
	}

	void precycle(EntityPool* pool) override {
		// Get cycle
		int cycle;
		registar->get("cycle", &cycle);

		// Push current cycle of ents
		transferBufferedEntsToGame(pool, cycle);
		// Execute buffered scripts
		executeBufferedCycleScripts(cycle);

		// Check if loading is requested
		bool loading = false;
		registar->get("loading", &loading);
		// If not loading exit
		if not(loading) {
			return;
		}

		string loadFolder = getLoadFolderPath();

		// Generate path to load table based on registar
		string path = getLoadTablePath(loadFolder);
		// Empty path is an error
		if (path == "") {
			registar->update("loading", false);
			return;
		}

		// Check for prelevel tables
		string preLevelPath = getPrelevelScript(loadFolder);
		if (preLevelPath.size()) {
			executeScript(os_kit::getFileAsString(preLevelPath));
		}

		// Check for templates
		string templatePath = getTemplateTablePath(loadFolder);
		if (templatePath.size()) {
			parseTemplateFile(templatePath);
		}

		/** 
		 * A load request has been evaluated by system_gamestate_controller, and has to be completed
		 */

		// Delete future ents
		deleteAllFutureEnts();

		if (!parseLoadTable(path)) {
			err::logMessage("LOAD: Fatal Error, load aborted");
		}
		else {
			err::logMessage("LOAD: Successful, starting level at " + path);
			registar->update("cycle", -1);
		}
		
		updateFlagsOnSuccessfulLoad();

		runStateChangeScript();

		executeScript(immediateScript);

		// Update to indicate loading successful
		registar->update("loading", false);
		registar->update("load_dialogue", true);
		registar->update("cycle_progress", true);

	}

};

#endif