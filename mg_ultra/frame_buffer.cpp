#include "frame_buffer.h"

#include <map>

const map<GLint, GLint> lookup = {
	{GL_R16F, GL_RED},
	{GL_R32F, GL_RED},
	{GL_RGB, GL_RGB},
	{GL_RGB12, GL_RGB},
	{GL_RGB16F, GL_RGB},
	{GL_RGB16_SNORM, GL_RGB},
	{GL_RGBA, GL_RGBA},
	{GL_RGBA16, GL_RGBA},
	{GL_RGBA16F, GL_RGBA},
};

GLint lookUpFormat(GLint internalFormat) {
	if (lookup.count(internalFormat)) {
		return lookup.find(internalFormat)->second;
	}
	else {
		throw GraphicsException("No input format in lookup table");
		return 0;
	}
}