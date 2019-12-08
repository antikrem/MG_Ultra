/*Simply a combined list of systems header, for easier adding*/
#ifndef __SYSTEM__HEADER__
#define __SYSTEM__HEADER__

//ring 0 - meta ECS management systems
#include "system_garbage_collector.h"
#include "system_bounds_control.h"
#include "system_multi_ent.h"
#include "system_profiler.h"

//ring 1 - non-gameplay essential systems
#include "system_animation.h"
#include "system_text.h"
#include "system_background.h"
#include "system_console.h"
#include "system_spawner.h"
#include "system_driftable.h"
#include "system_health.h"

//ring 2 - main game loop - most important main gameplay loop
#include "system_timer.h"
#include "system_player.h"
#include "system_camera.h"
#include "system_movement.h"
#include "system_rotation.h"
#include "system_game_state_control.h"

//ring 3 - loading
#include "system_loader.h"

//ring 4 - collision
#include "system_collision.h"


#endif