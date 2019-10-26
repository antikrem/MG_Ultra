/*Simply a combined list of systems header, for easier adding*/
#ifndef __SYSTEM__HEADER__
#define __SYSTEM__HEADER__

//ring 0 - meta ECS management systems
#include "system_garbage_collector.h"
#include "system_bounds_control.h"

//ring 1 - graphics pipeline system
#include "system_graphics.h"

//ring 2 - scripting
#include "system_script.h"

//ring 3 - non-gameplay essential systems
#include "system_animation.h"
#include "system_text.h"
#include "system_console.h"
#include "system_spawner.h"

//ring 4 - main game loop - most important main gameplay loop
#include "system_timer.h"
#include "system_player.h"
#include "system_camera.h"
#include "system_movement.h"
#include "system_game_state_control.h"

//ring 5 - loading
#include "system_loader.h"


#endif