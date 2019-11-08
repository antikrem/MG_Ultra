--"this" refers to player

--Player movement
PLAYER_ACCELERATION = 0.57
PLAYER_MAX_VELOCITY = 7.65
PLAYER_MAX_FOCUS_VELOCITY = 2.0
PLAYER_MAX_DASH_VELOCITY = 5.0
--max turn rate of player 
PLAYER_MAX_TURN_VELOCITY = 7;
--Theres a mechanic called quick shift
--if the player switches direction quickly its instant
--This is the threshold to active
PLAYER_QUICK_SHIFT_THRESHOLD = 70

--shooting timer
PLAYER_SHOOT_TIMING = 8

this:get_component(ComponentMovement):set_speed_cap(PLAYER_MAX_VELOCITY)
this:get_component(ComponentMovement):set_angle_cap(PLAYER_MAX_TURN_VELOCITY)