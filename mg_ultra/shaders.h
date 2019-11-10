/*Handles the encapsulation of shaders, 
including loading, compiling, using and deleting*/
#ifndef __SHADER__
#define __SHADER__

#define PATH_TO_SHADER "shaders/"

#include <string>
#include <map>

#include "_graphics_headers.h"

#include "frame_buffer.h"

#include "constants.h"
#include "str_kit.h"
#include "vec_kit.h"
#include "os_kit.h"

#include "error.h"

//Contains a single program
//and all the associated uniforms
struct ProgramDetails {
	GLuint programID;
	string programName;
	map<string, GLuint> uniforms;

	ProgramDetails(GLuint programID, string programName) {
		this->programID = programID;
		this->programName = programName;
	}

	//returns the uniform for a given
	//uniform name
	//if it doesnt exist will find
	//throws GraphicsException on error
	GLuint getUniformLocation(string name) {
		if (uniforms.count(name)) {
			return uniforms[name];
		}
		else {
			GLuint location;
			if ((location = glGetUniformLocation(programID, name.c_str())) < 0) {
				throw GraphicsException("Invalid uniform: " + name + " in program: " + programName);
			}
			else {
				uniforms[name] = location;
			}
			return location;
		}
		
	}
};


using ProgramMap_t = map<string, ProgramDetails>;

//Stores all the shaders, should only be one stored in gState
class ShaderMaster {
private:
	ProgramMap_t programMap;
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

		//insert new program to the map
		programMap.insert(ProgramMap_t::value_type(programName, ProgramDetails(programID, programName)));

		err::logMessage("GRAPHICS: Shader compilation done");
		err::logMessage("GRAPHICS: Shader program " + programName + " loaded");

		return EXIT_SUCCESS;
	}

	//Sets a shader program as current shader to use
	//throws exception on bad program name
	void useShader(string programName) {
		if (programMap.count(programName)) {

			glUseProgram(programMap.find(programName)->second.programID);
			currentProgram = programName;
			return;
		}
		else {
			throw GraphicsException("Invalid program name: " + programName + " requested for use");
		}
	}

	//Get the current program being used
	//returns zero on no program
	GLuint getShaderProgramID() {
		if (currentProgram == "") {
			return 0;
		}
		else {
			return programMap.find(currentProgram)->second.programID;
		}
	}

	//returns the uniform for a given
	//uniform name
	//if it doesnt exist will find
	//throws GraphicsException on error
	GLuint getUniformLocation(string programName, string uniformName) {
		if (programMap.count(programName)) {
			return programMap.find(programName)->second.getUniformLocation(uniformName);
		}
		else {
			throw GraphicsException("Invalid program: " + programName);
		}

	}

	//sets a uniform 4x4 matrix in the target shader
	void setUniform4f(string programName, string uniformName, glm::mat4& value) {
		glUniformMatrix4fv(getUniformLocation(programName, uniformName), 1, GL_FALSE, &value[0][0]);
	}

	//sets a variable number of uniform ints
	void setNUniformI(string programName, string uniformName, int n, vector<int>& values) {
		glUniform1iv(getUniformLocation(programName, uniformName), n, &values[0]);
	}

	//sets a single int in shader
	void setUniformI(string programName, string uniformName, int value) {
		glUniform1i(getUniformLocation(programName, uniformName), value);
	}

	//Attaches a framebuffer as the source for rendering
	//sampler is the name of the sampler array in the 
	void attachFrameBufferAsSource(string programName, FrameBuffer* frameBuffer) {
		auto colourTextures = frameBuffer->getColourTexture();
		auto targets = frameBuffer->getTargetNames();

		for (int i = 0; i < (int)targets.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, colourTextures[i]);
			setUniformI(programName, targets[i], i);
		}
	}

	ShaderMaster() {
		loadProgramFromFile("base");
		loadProgramFromFile("finalise");
	}

	~ShaderMaster() {
		glUseProgram(0);
		for (auto i : programMap) {
			glDeleteProgram(i.second.programID);
		}
	}

};


#endif