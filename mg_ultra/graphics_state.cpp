#include "graphics_state.h"
#include "events.h"

void windowUpdateCallback(GLFWwindow*, int x, int y) {
	cout << "callback received" << endl;
	g_events::clearEventQueue();
}