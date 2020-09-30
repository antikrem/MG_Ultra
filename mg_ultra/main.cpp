#include <stdio.h>
#include <stdlib.h>

#include "system.h"
#include "graphics_state.h"
#include "pool.h"


#include "thread_cruncher.h"

#include "ecs_master.h"

#include "error.h"

void glfwErrorCallback(int error, const char* description) {
	err::logMessage("WINDOW: error: %s\n" + string(description));
}

int main() {
	// Start error logger
	err::primeErrorFile();

	// Set error call back right at the start
	glfwSetErrorCallback(&glfwErrorCallback);

	ECSMaster* master = new ECSMaster();
	master->gameloop();
	delete master;

	// Stop global timers
	g_timer::finalise();

	// Ends the log
	err::endLog();

	return EXIT_SUCCESS;
}