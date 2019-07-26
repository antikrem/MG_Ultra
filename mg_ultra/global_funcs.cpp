#include "global_funcs.h"

#include <iostream>
#include <atomic>

#include <typeindex>

#include "events.h"

///print
//For use with print function, holds a buffer sting to print
string buffer;

void print_addString(const string& in) {
	buffer.append(in);
}

void print_addInt(int in) {
	buffer.append(to_string(in));
}

void print_addBool(bool in) {
	string message = in ? "true" : "false";
	buffer.append(message);
}

void print_push() {
	err::logMessage(buffer);
	buffer.clear();
}

//Factor event template which allows events to be constructed
Event eventConstruction(EV_noEvent);

//converts an int to a EventType
EventType convertIntToEventType(int flag) {
	if (flag < 0 || flag >= EV_LENGTH) {
		return EV_noEvent;
	}
	return (EventType)flag;
}

//sets event type
void setEvent(int type) {
	eventConstruction.type = convertIntToEventType(type);
	eventConstruction.data.clear();
}


void addEventData(string data) {
	eventConstruction.data.push_back(data);
}

void pushEvent() {
	g_events::pushEvent(eventConstruction);
}


void registerGlobalFunctions(kaguya::State &kaguya) {
	kaguya["printAdd"] = kaguya::overload(print_addString, print_addInt, print_addBool);
	kaguya["printPush"] = print_push;

	kaguya["setEvent"] = setEvent;
	kaguya["addEventData"] = addEventData;
	kaguya["pushEvent"] = pushEvent;
}