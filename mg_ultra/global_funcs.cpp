#include "global_funcs.h"

#include <iostream>
#include <atomic>
#include <mutex>

#include <typeindex>

#include "events.h"
#include "pool.h"
#include "texture.h"

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

///events
//Factory event template which allows events to be constructed
Event* eventConstruction = nullptr;

//converts an int to a EventType
EventType convertIntToEventType(int flag) {
	if (flag < 0 || flag >= EV_LENGTH) {
		return EV_noEvent;
	}
	return (EventType)flag;
}

//sets event type
void setEvent(int type) {
	if (!eventConstruction) {
		eventConstruction = new Event(convertIntToEventType(type));
		eventConstruction->data.clear();
	}
	else {
		//todo report error
	}
	
}


void addEventData(string data) {
	if (eventConstruction) {
		eventConstruction->data.push_back(data);
	}
	else {
		//todo report error
	}
}

void pushEvent() {
	if (eventConstruction) {
		g_events::pushEvent(eventConstruction);
		eventConstruction = nullptr;
	}
	else {
		//todo report error
	}
}

///profiling
mutex profileLock;
ProfileMap profileInfo;

ProfileMap getProfileInfoMap() {
	unique_lock<mutex> lck(profileLock);
	return profileInfo;
}

void setProfileInfoMap(ProfileMap& profileMap) {
	unique_lock<mutex> lck(profileLock);
	profileInfo = profileMap;
}

///animation master


void registerGlobalFunctions(kaguya::State &kaguya) {
	//printing
	kaguya["printAdd"] = kaguya::overload(print_addString, print_addInt, print_addBool);
	kaguya["printPush"] = print_push;

	//events
	kaguya["setEvent"] = setEvent;
	kaguya["addEventData"] = addEventData;
	kaguya["pushEvent"] = pushEvent;
	kaguya["get_event_length"] = g_events::queueSize;

	//profiling
	kaguya["getProfileInfo"] = getProfileInfoMap;

	//animation master
	kaguya["queryAnimation"] = g_aniquery::getAnimationSize;
}