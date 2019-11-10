#ifndef __FRAME_BUFFER__
#define __FRAME_BUFFER__

#include <vector>

#include "_graphics_headers.h"
#include "graphics_settings.h"

class FrameBuffer {
	unsigned int fbo;
	//vector of colour buffers
	vector<GLuint> drawTargets;
	//vector of the underlying colour buffer
	vector<GLuint> textureIDs;
	//vector of strings associated with draw targets
	vector<string> targetNames;

	//Creates an attaches a new colour buffer at the given level
	void createAndAttachColourBuffer(GraphicsSettings* graphicsSettings, int i, string name) {
		//Create colour buffer
		GLuint texColorBuffer;
		glGenTextures(1, &texColorBuffer);
		textureIDs.push_back(texColorBuffer);
		glBindTexture(GL_TEXTURE_2D, texColorBuffer);

		graphicsSettings->accessLock.lock();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, graphicsSettings->screenWidth, graphicsSettings->screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		graphicsSettings->accessLock.unlock();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texColorBuffer, 0);
		drawTargets.push_back(GL_COLOR_ATTACHMENT0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);

		targetNames.push_back(name);
	}

public:
	//returns true if creation was succesful
	void initialiseFrameBuffer(GraphicsSettings* graphicsSettings, vector<string> names, bool attachDepthBuffer) {
		glGenFramebuffers(1, &fbo);
		bindBuffer();

		//create all of the colour buffers desired
		for (unsigned int i = 0; i < names.size(); i++) {
			createAndAttachColourBuffer(graphicsSettings, i, names[i]);
		}
		glDrawBuffers((int)names.size(), &drawTargets[0]);

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
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			throw GraphicsException("Frame buffer failed creation");
		}
		unbindBuffer();
	}

	~FrameBuffer() {
		glDeleteFramebuffers(1, &fbo);
	}

	void bindBuffer() {
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		//depth test
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void unbindBuffer() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	//gets a vector of colour textures for sampling
	vector<GLuint>& getColourTexture() {
		return textureIDs;
	}

	//gets the vector of names
	vector<string>& getTargetNames() {
		return targetNames;
	}
};

#endif