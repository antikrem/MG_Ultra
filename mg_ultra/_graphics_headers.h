/*All the graphics/system apis need to be included in a specific way
to avoid macro redifinition*/

#ifndef __GRAPHICS_HEADERS__
#define __GRAPHICS_HEADERS__

#include <Windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <stdexcept>

struct GraphicsException : public std::runtime_error {
	GraphicsException(std::string error) : std::runtime_error(error) {

	}
};

//Use for debugging
void glEnableDebug();


#endif