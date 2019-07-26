// Include standard headers


#include <stdio.h>
#include <stdlib.h>

#include "system.h"
#include "graphics_state.h"
#include "pool.h"


#include "thread_cruncher.h"

#include "ecs_master.h"

#include "error.h"

#include "registar.h"



void glfwErrorCallback(int error, const char* description) {
	fprintf(stderr, "Error: %s\n", description);
}

int main() {

	Registar r;
	r.addToBase<int>("static", 10);
	int a;
	auto check = r.get<int>("static", &a);
	cout << a << endl;

	//set error call back right at the start
	glfwSetErrorCallback(&glfwErrorCallback);

	//start error logger
	err::primeErrorFile();

	ECSMaster* master = new ECSMaster();
	master->gameloop();
	delete master;

	//ends the log
	err::endLog();

	return EXIT_SUCCESS;
}