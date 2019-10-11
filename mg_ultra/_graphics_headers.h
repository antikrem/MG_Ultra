/*All the graphics/system apis need to be included in a specific way
to avoid macro redifinition*/

#ifndef __GRAPHICS_HEADERS__
#define __GRAPHICS_HEADERS__

#include <Windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//A standard gl error checking function
//will only work in debug mode

#include <iostream>

void glErrorSpam();


#endif