#include "graphics_state.h"
#include "events.h"

void windowUpdateCallback(GLFWwindow*, int x, int y) {
	g_events::clearEventQueue();
}