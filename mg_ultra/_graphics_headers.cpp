#include "_graphics_headers.h"

#include <stdio.h>

//TODO improve to use log

void GLAPIENTRY MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	fprintf(
		stderr, 
		"GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s, id = %d, source = 0x%x \n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, 
		severity, 
		message,
		id,
		source);
}

void glEnableDebug() {
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);
}