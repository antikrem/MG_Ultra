/*All the graphics/system apis need to be included in a specific way
to avoid macro redifinition*/

#ifndef __GRAPHICS_HEADERS__
#define __GRAPHICS_HEADERS__

#include <Windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//Use for debugging
void glEnableDebug();


#endif