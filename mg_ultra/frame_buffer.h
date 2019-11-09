#ifndef __FRAME_BUFFER__
#define __FRAME_BUFFER__

#include <vector>

#include "_graphics_headers.h"
#include "graphics_settings.h"

class FrameBuffer {
	unsigned int fbo;
	//vector of colour buffers
	vector<GLuint> drawTargets;

	//Creates an attaches a new colour buffer at the given level
	void createAndAttachColourBuffer(GraphicsSettings* graphicsSettings, int i) {
		//Create colour buffer
		GLuint texColorBuffer;
		glGenTextures(1, &texColorBuffer);
		glBindTexture(GL_TEXTURE_2D, texColorBuffer);

		graphicsSettings->accessLock.lock();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, graphicsSettings->screenWidth, graphicsSettings->screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		graphicsSettings->accessLock.unlock();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texColorBuffer, 0);
		drawTargets.push_back(GL_COLOR_ATTACHMENT0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

public:
	//returns true if creation was succesful
	bool initialiseFrameBuffer(GraphicsSettings* graphicsSettings, int colourBufferCount, bool attachDepthBuffer) {
		glGenFramebuffers(1, &fbo);
		bindBuffer();

		//create all of the colour buffers desired
		for (int i = 0; i < colourBufferCount; i++) {
			createAndAttachColourBuffer(graphicsSettings, i);
		}
		glDrawBuffers(colourBufferCount, &drawTargets[0]);

		if (attachDepthBuffer) {
			//Create depth buffer
			GLuint texRenderBuffer;
			glGenRenderbuffers(1, &texRenderBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, texRenderBuffer);

			graphicsSettings->accessLock.lock();
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, graphicsSettings->screenWidth, graphicsSettings->screenHeight);
			graphicsSettings->accessLock.unlock();

			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, texRenderBuffer);
		}
		
		//done, check and return
		bool result = (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
		unbindBuffer();
		return result;
	}

	~FrameBuffer() {
		glDeleteFramebuffers(1, &fbo);
	}

	void bindBuffer() {
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void unbindBuffer() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
};

#endif