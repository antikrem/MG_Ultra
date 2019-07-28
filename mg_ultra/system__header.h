/*Simply a combined list of systems header, for easier adding*/
#ifndef __SYSTEM__HEADER__
#define __SYSTEM__HEADER__

//ring 0 - meta ECS management systems
#include "system_garbage_collector.h"


//ring 1 - graphics pipeline system
#include "system_graphics.h"

//ring 2 - non-gameplay essential systems
#include "system_animation.h"
#include "system_text.h"
#include "system_console.h"

//ring 3 - main game loop - most important main gameplay loop
#include "system_game_state_control.h"
#include "system_script.h"

//ring 4 - loading
#include "system_loader.h"

#endif