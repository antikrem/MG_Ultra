/*Simply a combined list of systems header, for easier adding*/
#ifndef __SYSTEM__HEADER__
#define __SYSTEM__HEADER__

//ring 0 - meta ECS management systems
#include "system_garbage_collector.h"
#include "system_bounds_control.h"
#include "system_profiler.h"

//ring 0 - higher meta ECS management systems
#include "system_multi_ent.h"
#include "system_bullet_master.h"
#include "system_bullet_spawner.h"
#include "system_die_with_master.h"

//ring 2 - non-gameplay essential systems
#include "system_animation.h"
#include "system_text.h"
#include "system_background.h"
#include "system_console.h"
#include "system_spawner.h"
#include "system_driftable.h"
#include "system_health.h"
#include "system_transparency.h"
#include "system_weather.h"
#include "system_point_lighting_update.h"
#include "system_colour_modulation.h"
#include "system_ambient_light.h"
#include "system_fog.h"

//ring 3 - main game loop - most important main gameplay loop
#include "system_timer.h"
#include "system_player.h"
#include "system_camera.h"
#include "system_movement.h"
#include "system_rotation.h"
#include "system_game_state_control.h"

//ring 4 - loading
#include "system_loader.h"

//ring 5 - collision
#include "system_collision.h"

//ring 6 - audio
#include "system_audio.h"
#include "system_music.h"

//ring 7 - particles
#include "system_particle_spawner.h"
#include "system_particle_boxes.h"
#include "system_force_applier.h"
#include "system_particle_update.h"

#endif