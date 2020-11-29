/*Handles the encapsulation of shaders, 
including loading, compiling, using and deleting*/
#ifndef __SHADER__
#define __SHADER__

#define PATH_TO_SHADER "shaders/"

#include <string>
#include <map>
#include <regex>

#include "_graphics_headers.h"

#include "frame_buffer.h"

#include "constants.h"
#include "algorithm_ex.h"
#include "str_kit.h"
#include "vec_kit.h"
#include "os_kit.h"

#include "error.h"

#include <iostream>

//capture pattern for #includes
#define INCLUDE_CAPTURE_PATTERN "#include \"(.*)\""

//Contains a single program
//and all the associated uniforms
struct ProgramDetails {
	GLuint programID;
	string programName;
	map<string, GLint> uniforms;

	ProgramDetails(GLuint programID, string programName) {
		this->programID = programID;
		this->programName = programName;
	}

	//returns the uniform for a given
	//uniform name
	//if it doesnt exist will find
	//throws GraphicsException on error
	GLint getUniformLocation(string name, bool failSilently = false) {
		if (uniforms.count(name)) {
			return uniforms[name];
		}
		else {
			GLint location;
			if ((location = glGetUniformLocation(programID, name.c_str())) < 0) {
				if (failSilently) {
					return -1;
				}
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

	vector<string> includes;

	bool debug = false;

	//conducts the internal inclusion recusivly
	bool internalHandleIncludes(string& input) {
		smatch locations;
		if (regex_search(input, locations, std::regex(INCLUDE_CAPTURE_PATTERN))) {
			auto includedfile = locations[1].str();

			//if already included return false and error
			if (contains(includes, includedfile)) {
				err::logMessage("GRAPHICS: Fatal Error, recursion error parsing includes in: " + includes[0] + "\n"
					+ "Error was in including " + includedfile);
				return false;
			}

			//otherwise add to list
			includes.push_back(includedfile);

			//get contents
			string includedContents = os_kit::getFileAsString(PATH_TO_SHADER + includedfile);
			if (!includedContents.size()) {
				err::logMessage("GRAPHICS: Fatal Error, was not able to open: " PATH_TO_SHADER + includes[0]);
				return false;
			}

			//replace include with file
			input.replace(locations.position(), locations[0].length(), includedContents);

			//continue scanning
			return internalHandleIncludes(input);

		}
		return true;
	}

	//a simple function to handle #include
	//returns bool on sucess, otherwise false
	bool handleIncludes(string& input, const string& parseFile) {
		includes.clear();
		includes.push_back(parseFile);
		return internalHandleIncludes(input);
	}

public:
	//Loads program, returns EXIT value, and success will update programMap
	int loadProgramFromFile(string programName) {
		err::logMessage("GRAPHICS: Compiling shader program: " + programName + "...");
		// Create the shaders
		GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
		string vertPath = PATH_TO_SHADER + programName + ".vert";
		string fragPath = PATH_TO_SHADER + programName + ".frag";

		// Read the Vertex Shader code from the file
		std::string vertexShaderCode = os_kit::getFileAsString(vertPath);
		if not(vertexShaderCode.size()) {
			err::logMessage("GRAPHICS: ERROR, Failed to load vertex shader at " + vertPath);
			return EXIT_FAILURE;
		}

		if not(handleIncludes(vertexShaderCode, programName + ".vert")) {
			return EXIT_FAILURE;
		}

		// Read the Fragment Shader code from the file
		std::string fragmentShaderCode = os_kit::getFileAsString(fragPath);
		if not(fragmentShaderCode.size()) {
			err::logMessage("GRAPHICS: ERROR, Failed to load fragment shader at " + fragPath);
			return EXIT_FAILURE;
		}

		if not(handleIncludes(fragmentShaderCode, programName + ".frag")) {
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
			err::logMessage("GRAPHICS: ERROR, Failed to compile vertex shader: " + vertPath);
			err::logMessage("Compilation log: " + string(vertexShaderErrorMessage.begin(), vertexShaderErrorMessage.end()));
			return EXIT_FAILURE;
		}
		glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result);
		glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (!result) {
			std::vector<char> fragmentShaderErrorMessage(infoLogLength + 1);
			glGetShaderInfoLog(fragmentShaderID, infoLogLength, NULL, &fragmentShaderErrorMessage[0]);
			err::logMessage("GRAPHICS: ERROR, Failed to compile fragment shader: " + fragPath);
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
			err::logMessage("GRAPHICS: ERROR, Failed to link program: " + fragPath);
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
	GLint getUniformLocation(string programName, string uniformName, bool failSilently = false) {
		if (programMap.count(programName)) {
			return programMap.find(programName)->second.getUniformLocation(uniformName, failSilently || !debug);
		}
		else {
			throw GraphicsException("Invalid program: " + programName);
		}

	}

	//sets a uniform 4x4 matrix in the target shader
	void setUniformMatrix4F(string programName, string uniformName, const glm::mat4& value) {
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

	//sets a single int in a failing manner
	//has no effect on invalid values
	void setUniformINoException(string programName, string uniformName, int value) {
		GLint location = getUniformLocation(programName, uniformName, true);
		if not(location < 0) {
			glUniform1i(location, value);
		}
	}

	//sets a single float in shader
	void setUniformF(string programName, string uniformName, float value) {
		glUniform1f(getUniformLocation(programName, uniformName), value);
	}

	//sets a uniform vec2
	void setUniform2F(string programName, string uniformName, const glm::vec2& value) {
		glUniform2f(getUniformLocation(programName, uniformName), value[0], value[1]);
	}

	//sets a uniform vec3
	void setUniform3F(string programName, string uniformName, const glm::vec3& value) {
		glUniform3f(getUniformLocation(programName, uniformName), value[0], value[1], value[2]);
	}

	//sets a uniform float array
	void setUniform1FV(string programName, string uniformName, int length, float* values) {
		glUniform1fv(getUniformLocation(programName, uniformName), length, values);
	}

	//sets a uniform bool
	void setUniformBool(string programName, string uniformName, bool value) {
		glUniform1i(getUniformLocation(programName, uniformName), (int)value);
	}

	//Attaches a framebuffer as the source for rendering
	//sampler is the name of the sampler array in the
	//returns the next starting TU, and takes an optional starting TU
	int attachFrameBufferAsSource(string programName, FrameBuffer* frameBuffer, int start = 0) {
		auto colourTextures = frameBuffer->getColourTexture();
		auto targets = frameBuffer->getTargetNames();

		int i = start;
		for (; i < (int)targets.size() + start; i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, colourTextures[i - start]);
			setUniformINoException(programName, targets[i - start], i);
		}
		return i + 1;
	}

	//attaches a single texture as source for rendering
	//takes and returns chaining
	int attachTextureAsSource(string programName, string uniformName, GLint texID, int start = 0) {
		glActiveTexture(GL_TEXTURE0 + start);
		glBindTexture(GL_TEXTURE_2D, texID);
		setUniformINoException(programName, uniformName, start);

		return start + 1;
	}

	ShaderMaster() {
		loadProgramFromFile("base");
		loadProgramFromFile("front_depth_buffer");
		loadProgramFromFile("ui");
		loadProgramFromFile("directional_lighting");
		loadProgramFromFile("point_lighting");
		loadProgramFromFile("unified_lighting");
		loadProgramFromFile("bloom");
		loadProgramFromFile("gauss");
		loadProgramFromFile("colour_correction");
	}

	~ShaderMaster() {
		glUseProgram(0);
		for (auto i : programMap) {
			glDeleteProgram(i.second.programID);
		}
	}

};


#endif