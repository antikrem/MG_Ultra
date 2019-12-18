#ifndef __FRAME_BUFFER__
#define __FRAME_BUFFER__

#include <vector>

#include "_graphics_headers.h"
#include "graphics_settings.h"
#include "algorithm_ex.h"

struct BufferSpecification {
	string name;
	GLint internalFormat;

	BufferSpecification(string name, GLint internalFormat) {
		this->name = name;
		this->internalFormat = internalFormat;
	}
};

//provides lookup for format given an internal format
GLint lookUpFormat(GLint internalFormat);

//enumeration of depth buffer attachment options
enum DepthAttachmentOptions {
	ATTACH_NONE = 0b00,
	DEPTH_DEFAULT = 0b01,
	DEPTH_24 = 0b10,

};

class FrameBuffer {
	unsigned int fbo;
	//vector of colour buffers
	vector<GLuint> drawTargets;
	//vector of the underlying colour buffer
	vector<GLuint> textureIDs;
	//vector of strings associated with draw targets
	vector<string> targetNames;

	//depth buffer texture identifier, -1(wrapped) on no texture id
	GLuint depthTexID = -1;

	//Creates an attaches a new colour buffer at the given level
	void createAndAttachColourBuffer(GraphicsSettings* graphicsSettings, int i, BufferSpecification specification) {
		//Create colour buffer
		GLuint texColorBuffer;
		glGenTextures(1, &texColorBuffer);
		textureIDs.push_back(texColorBuffer);
		glBindTexture(GL_TEXTURE_2D, texColorBuffer);

		graphicsSettings->accessLock.lock();
		glTexImage2D(
			GL_TEXTURE_2D, 
			0, 
			specification.internalFormat,
			graphicsSettings->screenWidth, 
			graphicsSettings->screenHeight, 
			0, 
			lookUpFormat(specification.internalFormat),
			GL_FLOAT,
			NULL
		);
		graphicsSettings->accessLock.unlock();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texColorBuffer, 0);
		drawTargets.push_back(GL_COLOR_ATTACHMENT0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);

		targetNames.push_back(specification.name);
	}

public:
	//returns true if creation was succesful
	//takes bit ored combination of DepthAttachmentOptions
	void initialiseFrameBuffer(GraphicsSettings* graphicsSettings, vector<BufferSpecification> specifications, DepthAttachmentOptions attachDepthBuffer) {
		glGenFramebuffers(1, &fbo);
		bindBuffer();

		//create all of the colour buffers desired
		for (unsigned int i = 0; i < specifications.size(); i++) {
			createAndAttachColourBuffer(graphicsSettings, i, specifications[i]);
		}
		glDrawBuffers((int)specifications.size(), &drawTargets[0]);

		if (attachDepthBuffer) {
			//Create depth buffer
			if (attachDepthBuffer & DepthAttachmentOptions::DEPTH_DEFAULT) {
				glGenRenderbuffers(1, &depthTexID);
				glBindRenderbuffer(GL_RENDERBUFFER, depthTexID);

				graphicsSettings->accessLock.lock();
				glRenderbufferStorage(
					GL_RENDERBUFFER, 
					GL_DEPTH_COMPONENT, 
					graphicsSettings->screenWidth, 
					graphicsSettings->screenHeight
				);
				graphicsSettings->accessLock.unlock();

				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthTexID);
			}
			else {
				glGenTextures(1, &depthTexID);
				glBindTexture(GL_TEXTURE_2D, depthTexID);

				graphicsSettings->accessLock.lock();
				glTexImage2D(
					GL_TEXTURE_2D, 
					0, 
					GL_DEPTH_COMPONENT24, 
					graphicsSettings->screenWidth, 
					graphicsSettings->screenHeight, 
					0, 
					GL_DEPTH_COMPONENT, 
					GL_UNSIGNED_INT, 
					nullptr
				);
				graphicsSettings->accessLock.unlock();

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexID, 0);
			}
			
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
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
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

	//clears depth buffer of this frame buffer
	void clearDepthBuffer() {
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	//only valid after binding
	//specifies a blend function for a specified buffer 
	void setBlendFunction(const string& buffer, GLenum mode, GLenum sfactor, GLenum dfactor) {
		int location = index_of(targetNames, buffer);
		if (location < 0) {
			throw GraphicsException("Invalid buffer in in setBlendFunction");
		}
		else {
			glBlendEquationi(location, mode);
			glBlendFunci(location, sfactor, dfactor);
		}
	}


	//disables all blending
	void disableBlending() {
		glDisable(GL_BLEND);
	}
};

#endif