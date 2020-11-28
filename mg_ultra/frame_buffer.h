#ifndef __FRAME_BUFFER__
#define __FRAME_BUFFER__

#include <vector>
#include <tuple>

#include "error.h"

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
	ATTACH_NONE = 0b000,
	DEPTH_DEFAULT = 0b001,
	DEPTH_24 = 0b010,
	DEPTH_32 = 0b110,
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
	
	//width and length of framebuffer in pixels
	int width;
	int height;
	//resolution factor against the resolution find in gSettings
	float resolution = 1.0f;
	
	//Creates an attaches a new colour buffer at the given level
	void createAndAttachColourBuffer(GraphicsSettings* graphicsSettings, int i, BufferSpecification specification) {
		//Create colour buffer
		GLuint texColorBuffer;
		glGenTextures(1, &texColorBuffer);
		textureIDs.push_back(texColorBuffer);
		glBindTexture(GL_TEXTURE_2D, texColorBuffer);

		glTexImage2D(
			GL_TEXTURE_2D, 
			0, 
			specification.internalFormat,
			width,
			height,
			0, 
			lookUpFormat(specification.internalFormat),
			GL_FLOAT,
			NULL
		);

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
	void initialiseFrameBuffer(GraphicsSettings* graphicsSettings, vector<BufferSpecification> specifications, DepthAttachmentOptions attachDepthBuffer, float resolution = 1.0f) {
		glGenFramebuffers(1, &fbo);
		
		this->resolution = resolution;
		
		this->width = (int)(graphicsSettings->screenWidth * resolution);
		this->height = (int)(graphicsSettings->screenHeight * resolution);
		
		bindBuffer(false);

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

				glRenderbufferStorage(
					GL_RENDERBUFFER, 
					GL_DEPTH_COMPONENT, 
					width, 
					height
				);

				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthTexID);
			}
			else if (attachDepthBuffer & DepthAttachmentOptions::DEPTH_24) {
				glGenTextures(1, &depthTexID);
				glBindTexture(GL_TEXTURE_2D, depthTexID);

				glTexImage2D(
					GL_TEXTURE_2D, 
					0, 
					attachDepthBuffer == DEPTH_32 ? GL_DEPTH_COMPONENT32 : GL_DEPTH_COMPONENT24,
					width, 
					height,
					0, 
					GL_DEPTH_COMPONENT, 
					GL_UNSIGNED_INT, 
					nullptr
				);

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexID, 0);
			}
			else {
				throw GraphicsException("Invalid enum");
			}
			
		}
		
		//done, check and return
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			err::logMessage("GL_HANDLER: could not complete framebuffer");
			throw GraphicsException("Frame buffer failed creation");
		}
		unbindBuffer();
	}

	~FrameBuffer() {
		glDeleteFramebuffers(1, &fbo);
	}
	
	//gets fbo id
	unsigned int getID() {
		return fbo;
	}
	
	//gets resolution scale
	float getResolutionFactor() {
		return resolution;
	}
	
	//get resolution
	tuple<int, int> getResolution() {
		return make_tuple(width, height);
	}

	//binds and clears depth buffer
	void bindBuffer(bool clear = true) {
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		// depth test
		if (clear) {
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

	}

	//binds with no clear
	void bindNoClearBuffer() {
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
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

	//clears the buffer
	void clearBuffer() {
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

	//only valid after binding
	//specifies a blend function for a specified buffer 
	//colour and alpha are set seperatly
	void setBlendFunctionSeperate(const string& buffer, GLenum mode, GLenum sRGBfactor, GLenum dRGBfactor, GLenum sAlphafactor, GLenum dAlphafactor) {
		int location = index_of(targetNames, buffer);
		if (location < 0) {
			throw GraphicsException("Invalid buffer in setBlendFunction");
		}
		else {
			glBlendEquationi(location, mode);
			glBlendFuncSeparatei(location, sRGBfactor, dRGBfactor, sAlphafactor, dAlphafactor);
		}
	}

	//returns the depth tex id
	GLint getDepthTextID() {
		return (GLint)depthTexID;
	}

	//disables all blending
	void disableBlending() {
		glDisable(GL_BLEND);
	}
	
	//copies to this fbo from another
	void copyIn(FrameBuffer& source, GLenum mask = GL_COLOR_BUFFER_BIT, GLenum filter = GL_LINEAR) {
		int sWidth, sHeight;
		tie(sWidth, sHeight) = source.getResolution();
		
		glBlitNamedFramebuffer(
			source.getID(), fbo,
			0, 0, sWidth, sHeight,
  			0, 0, width, height,
  			mask,
  			filter
  		);
	}
};

#endif
