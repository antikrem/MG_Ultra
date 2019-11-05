/*The interface between gamestate and GLHandler,
Doesnt actually do much gl tho*/
#include <mutex>

#include "shaders.h"
#include "constants.h"
#include "gl_handler.h"

#ifndef __GRAPHICS_STATE__
#define __GRAPHICS_STATE__

class GraphicsState {
private:
	//Graphics settings
	GraphicsSettings* gSettings = new GraphicsSettings();

	//Handles GL calls and interaction with the gl interface
	GLHandler* glHandler = nullptr;

	//camera object
	Camera* camera = nullptr;

	//Texture master
	AnimationsMaster* animationsMaster = nullptr;
	
	//creates window returns EXIT_FAILURE on fail, else EXIT_SUCCESS
	int createWindowAndPassToGLHandler() {
		// Initialise GLFW
		if (!glfwInit()) {
			//TODO fprintf(stderr, "Failed to initialize GLFW\n");
			return EXIT_FAILURE;
		}

		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// Open a window and create its OpenGL context
		GLFWwindow *window = glfwCreateWindow(gSettings->screenWidth, gSettings->screenHeight, "mg_ultra", NULL, NULL);
		if (window == NULL) {
			glfwTerminate();
			return EXIT_FAILURE;
		}

		//Make context current for a bit on initialise thread
		glfwMakeContextCurrent(window);
		glewExperimental = true;
		if (glewInit() != GLEW_OK) {
			fprintf(stderr, "Failed to initialize GLEW\n");
			return EXIT_FAILURE;
		}

		//create camera
		camera = new Camera(gSettings);
		
		//create texture master
		animationsMaster = new AnimationsMaster();

		//Switch over to the GLHandler
		glHandler = new GLHandler(window, gSettings, animationsMaster, camera);
		glHandler->blockUntilActive();

		return EXIT_SUCCESS;
	}

public:
	//Creates graphic states and initialises openGL
	GraphicsState() {
		//create a window and pass it off to GLHander
		if FAILURE(createWindowAndPassToGLHandler()) {
			//TODO
		}
	}

	Camera* getCamera() {
		return camera;
	}

	~GraphicsState() {
		delete glHandler;
		delete camera;
		delete gSettings;
	}

	GLFWwindow* getWindow() {
		return glHandler->getWindow();
	}

	AnimationsMaster* getAnimationsMaster() {
		return animationsMaster;
	}

	unsigned long long int getNextUnmap() {
		return glHandler->getNextUnmap();
	}

	BoxData evaluateToBox(AnimationState state, float scale) {
		return animationsMaster->evaluateToBox(state, scale);
	}

	//gets a pointer to a buffered map
	//blocks until fufiled
	//also commits the last block
	BoxData* getBoxDataBuffer() {
		return glHandler->getBoxDataBuffer();
	}

	//commit the buffer, waiting until operation is complete
	void commitBoxDataBuffer(int boxCount) {
		glHandler->commitBoxDataBuffer(boxCount);
	}

	//returns the size of the GLHandler's boxBufferDataBuffer size
	int getBoxDataBufferSize() {
		return glHandler->getBoxDataBufferSize();
	}
};


#endif // ifndef GRAPHICS_STATE