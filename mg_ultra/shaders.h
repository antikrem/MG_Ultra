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
		std::string vertexShaderCode = str_kit::getContentsOfFile(vertPath);
		if FAILURE( str_kit::getErrorCode() ) {
			err::logMessage("GRAPHICS: Failed to load vertex shader at " + vertPath);
			//TODO
			return EXIT_FAILURE;
		}

		// Read the Fragment Shader code from the file
		std::string fragmentShaderCode = str_kit::getContentsOfFile(fragPath);
		if FAILURE( str_kit::getErrorCode() ) {
			err::logMessage("GRAPHICS: Failed to load fragment shader at " + fragPath);
			//TODO
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
		if (infoLogLength > 0) {
			std::vector<char> fragmentShaderErrorMessage(infoLogLength + 1);
			glGetShaderInfoLog(vertexShaderID, infoLogLength, NULL, &fragmentShaderErrorMessage[0]);
			printf("%s\n", &fragmentShaderErrorMessage[0]);
			return EXIT_FAILURE;
		}
		glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result);
		glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0) {
			std::vector<char> fragmentShaderErrorMessage(infoLogLength + 1);
			glGetShaderInfoLog(fragmentShaderID, infoLogLength, NULL, &fragmentShaderErrorMessage[0]);
			printf("%s\n", &fragmentShaderErrorMessage[0]);
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
		if (infoLogLength > 0) {
			std::vector<char> ProgramErrorMessage(infoLogLength + 1);
			glGetProgramInfoLog(programID, infoLogLength, NULL, &ProgramErrorMessage[0]);
			printf("%s\n", &ProgramErrorMessage[0]);
			return EXIT_FAILURE;
		}

		glDetachShader(programID, vertexShaderID);
		glDetachShader(programID, fragmentShaderID);

		glDeleteShader(vertexShaderID);
		glDeleteShader(fragmentShaderID);

		programMap[programName] = programID;

		err::logMessage("GRAPHICS: Shader compilation done");

		return EXIT_SUCCESS;
	}

	//Sets a shader program as current shader to use
	int useShader(string programName) {
		if (programMap.count(programName)) {
			glUseProgram(programMap[programName]);
			currentProgram = programName;
			err::logMessage("GRAPHICS: Shader program " + programName + " loaded");
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