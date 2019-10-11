#include "_graphics_headers.h"


void glErrorSpam() {
	GLenum errorCode;
	bool errored = false;
	std::cout << "GL ERROR REPORT" << std::endl;
	while ((errorCode = glGetError()) != GL_NO_ERROR) {
		errored = true;
		std::cout << glGetString(errorCode) << std::endl;
	}
	if (!errored) {
		std::cout << "GL NO ERROR " << std::endl;
	}

}

