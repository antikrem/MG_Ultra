/*Handles the encapsulation of shaders, 
including loading, compiling, using and deleting*/
#ifndef __SHADER__
#define __SHADER__

#define PATH_TO_SHADER "shaders/"

#include <string>
#include <map>

#include "_graphics_headers.h"

#include "constants.h"
#include "str_kit.h"
#include "os_kit.h"
#include "error.h"

//Stores all the shaders, should only be one stored in gState
class ShaderMaster {
private:
	map<string, GLuint> programMap;
	string currentProgram = "";

public:
	//Loads program, returns EXIT value, and success will update programMap
	int loadProgramFromFile(string programName) {
		err::logMessage("GRAPHICS: Compilling shader program: " + programName + "...");
		// Create the shaders
		GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
		string vertPath = PATH_TO_SHADER + programName + ".vert";
		string fragPath = PATH_TO_SHADER + programName + ".frag";

		// Read the Vertex Shader code from the file
		std::string vertexShaderCode = os_kit::getFileAsString(vertPath);
		if not(vertexShaderCode.size()) {
			err::logMessage("GRAPHICS: Failed to load vertex shader at " + vertPath);
			return EXIT_FAILURE;
		}

		// Read the Fragment Shader code from the file
		std::string fragmentShaderCode = os_kit::getFileAsString(fragPath);
		if not(fragmentShaderCode.size()) {
			err::logMessage("GRAPHICS: Failed to load fragment shader at " + fragPath);
			return EXIT_FAILURE;
		}

		// Compile Vertex Shader
		char const *vertexSourcePointer = vertexShaderCode.c_str();
		glShaderSource(vertexShaderID, 1, &vertexSourcePointer, NULL);
		glCompileShader(vertexShaderID);
		// Compile Fragment Shader
		char const *fragmentSourcePointer = fragmentShaderCode.c_str();
		glShaderSource(fragmentShaderID, 1, &fragmentSourcePointer, NULL);
		glCompileShader(fragmentShaderID);

		//Check shaders
		GLint result = GL_FALSE;
		int infoLogLength;
		glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result);
		glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (!result) {
			std::vector<char> vertexShaderErrorMessage(infoLogLength + 1);
			glGetShaderInfoLog(vertexShaderID, infoLogLength, NULL, &vertexShaderErrorMessage[0]);
			err::logMessage("GRAPHICS: Fatal Error, Failed to compile vertex shader: " + vertPath);
			err::logMessage("Compilation log: " + string(vertexShaderErrorMessage.begin(), vertexShaderErrorMessage.end()));
			return EXIT_FAILURE;
		}
		glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result);
		glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (!result) {
			std::vector<char> fragmentShaderErrorMessage(infoLogLength + 1);
			glGetShaderInfoLog(fragmentShaderID, infoLogLength, NULL, &fragmentShaderErrorMessage[0]);
			err::logMessage("GRAPHICS: Fatal Error, Failed to compile fragment shader: " + fragPath);
			err::logMessage("Compilation log: " + string(fragmentShaderErrorMessage.begin(), fragmentShaderErrorMessage.end()));
			return EXIT_FAILURE;
		}

		//Link programs
		GLuint programID = glCreateProgram();
		glAttachShader(programID, vertexShaderID);
		glAttachShader(programID, fragmentShaderID);
		glLinkProgram(programID);

		//Check Program
		glGetProgramiv(programID, GL_LINK_STATUS, &result);
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (!result) {
			std::vector<char> ProgramErrorMessage(infoLogLength + 1);
			glGetProgramInfoLog(programID, infoLogLength, NULL, &ProgramErrorMessage[0]);
			err::logMessage("GRAPHICS: Fatal Error, Failed to link program: " + fragPath);
			err::logMessage("Compilation log: " + string(ProgramErrorMessage.begin(), ProgramErrorMessage.end()));
			return EXIT_FAILURE;
		}

		glDetachShader(programID, vertexShaderID);
		glDetachShader(programID, fragmentShaderID);

		glDeleteShader(vertexShaderID);
		glDeleteShader(fragmentShaderID);

		programMap[programName] = programID;

		err::logMessage("GRAPHICS: Shader compilation done");
		err::logMessage("GRAPHICS: Shader program " + programName + " loaded");

		return EXIT_SUCCESS;
	}

	//Sets a shader program as current shader to use
	int useShader(string programName) {
		if (programMap.count(programName)) {
			glUseProgram(programMap[programName]);
			currentProgram = programName;
			return EXIT_SUCCESS;
		}
		else {
			err::logMessage("GRAPHICS: Shader program " + programName + " not found");
			return EXIT_FAILURE;
		}
	}

	//Get the current program being used
	GLuint getShaderProgramID() {
		if (currentProgram == "") {
			return 0;
		}
		else {
			return programMap[currentProgram];
		}
	}

	ShaderMaster() {
		loadProgramFromFile("base");
		useShader("base");
	}

	~ShaderMaster() {
		glUseProgram(0);
		for (auto i : programMap) {
			glDeleteProgram(i.second);
		}
	}

};


#endif