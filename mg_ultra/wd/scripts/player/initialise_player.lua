-- scripts\initialise_player.lua

-- Variables used to control player movement
this:get_component(ComponentMovement):set_speed_cap(PLAYER_MAX_VELOCITY_TABLE["DEFAULT"])
this:get_component(ComponentMovement):set_angle_cap(PLAYER_MAX_TURN_VELOCITY)

-- Colour of player light
-- The light is activated by other systems (mostly weather)
this:get_component(ComponentPointLight):set_colour(0.9647, 0.8039, 0.3751)

-- Set this audio component to play shoot tick
this:get_component(ComponentAudio):set_source("player_shoot_tick")
this:get_component(ComponentAudio):set_repeat(true)

--Make new player scroll in
this:get_component(ComponentClampPosition):set_active(false)
this:get_component(ComponentPosition):set_position(0, -640)
g_sequentialDeadFrames = 0
GlobalRegistar.update("player_alive", false)

-- Set player lighting
Weather.update_player_light()

--Make player visible
this:get_component(ComponentGraphics):set_visible(true)