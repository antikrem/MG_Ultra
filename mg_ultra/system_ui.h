#ifndef __SYSTEM_UI__
#define __SYSTEM_UI__

#include "system.h"

#include "ui.h"

class SystemUI : public System {

public:
	SystemUI() {
		debugName = "s_ui";
		//empty system
		target = SubPoolTarget();
	}

	void precycle(EntityPool* pool) override {
		g_ui::updateUI();
	}

};

#endif