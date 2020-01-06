--"this" refers to player

--Player movement
PLAYER_ACCELERATION = 0.97
PLAYER_DASH_ACCELERATION = 5.3
PLAYER_MAX_VELOCITY = 12.65
PLAYER_MAX_FOCUS_VELOCITY = 2.0
PLAYER_MAX_DASH_VELOCITY = 33.0

--Dash related variables
PLAYER_DASH_LENGTH = 10
PLAYER_DASH_COOLDOWN = 14


--max turn rate of player 
PLAYER_MAX_TURN_VELOCITY = 7;
--Theres a mechanic called quick shift
--if the player switches direction quickly its instant
--This is the threshold angle to active
PLAYER_QUICK_SHIFT_THRESHOLD = 70

--shooting timer
PLAYER_SHOOT_TIMING = 8

this:get_component(ComponentMovement):set_speed_cap(PLAYER_MAX_VELOCITY)
this:get_component(ComponentMovement):set_angle_cap(PLAYER_MAX_TURN_VELOCITY)

--values that represent the last dash direction
g_lx, g_ly = 0,0
--set to true when a dash is occuring
g_inDash = false
--set to true when the dash key is released
g_dashReleased = true
--time spent in dash
g_dashDuration = 0
--time spent in cooldown
g_dashCooldown = 0