-- scripts\initialise_player.lua

--Registar value to control
GlobalRegistar.add("player_alive", true)

-- Player movement
PLAYER_ACCELERATION = 0.97
PLAYER_DASH_ACCELERATION = 5.3
PLAYER_MAX_VELOCITY = 12.65
PLAYER_MAX_FOCUS_VELOCITY = 2.0
PLAYER_MAX_DASH_VELOCITY = 33.0

-- Additional soft position clamping
PLAYER_X_CLAMP = 960
PLAYER_Y_CLAMP = 540

-- Dash related variables
PLAYER_DASH_LENGTH = 16
PLAYER_DASH_COOLDOWN = 5

-- Constants for dead player
PLAYER_DEAD_SCROLL = 70
PLAYER_DEAD_SCROLL_START_Y = -640
PLAYER_DEAD_SCROLL_END_Y = -270

-- Max turn rate of player 
PLAYER_MAX_TURN_VELOCITY = 7;
-- Theres a mechanic called quick shift
-- If the player switches direction quickly its instant
-- This is the threshold angle to active
PLAYER_QUICK_SHIFT_THRESHOLD = 70

-- shooting timer
PLAYER_SHOOT_TIMING = 7

-- Variables used to control player movement
this:get_component(ComponentMovement):set_speed_cap(PLAYER_MAX_VELOCITY)
this:get_component(ComponentMovement):set_angle_cap(PLAYER_MAX_TURN_VELOCITY)

-- Colour of player light
-- The light is activated by other systems (mostly weather)
this:get_component(ComponentPointLight):set_colour(0.9647, 0.8039, 0.3751)

-- Counter for number of dead frames
g_sequentialDeadFrames = 0



-- Values that represent the last dash direction
g_lx, g_ly = 0,0
-- Set to true when a dash is occuring
g_inDash = false
-- Set to true when the dash key is released
g_dashReleased = true
-- Time spent in dash
g_dashDuration = 0
-- Time spent in cooldown
g_dashCooldown = 0

-- Functions used to update player during regular gameplay
g_playerMovementUpdate = function()
	--The position of the player
	local cPosition = this:get_component(ComponentPosition)
	--More sophisticated interface for controlling movement
	local cMovement = this:get_component(ComponentMovement)
	--A component to get player input
	local cInput = this:get_component(ComponentInput)

	--represents input
	local ix, iy = 0,0

	if cInput:query_down("left")  == 1 then ix = ix - 1 end
	if cInput:query_down("right") == 1 then ix = ix + 1 end
	if cInput:query_down("up")    == 1 then iy = iy + 1 end
	if cInput:query_down("down")  == 1 then iy = iy - 1 end

	--Update dash variables
	if g_dashCooldown > 0 then g_dashCooldown = g_dashCooldown - 1 end
	if g_dashDuration > 0 then g_dashDuration = g_dashDuration - 1 end

	--if currently dashing
	if g_inDash then	
		--check for dash conditions to fail
		if ix ~= lx or iy ~= ly or g_dashDuration <= 0 then
			g_dashDuration = 0
			g_inDash = false
			g_dashCooldown = PLAYER_DASH_COOLDOWN
		end
	end

	--check if key input will allow for dash
	if cInput:query_down("dash") == 1 then
		--handle case where dash input recieved out of dash 
		if not g_inDash and g_dashReleased and g_dashCooldown <= 0 then
			g_inDash = true
			lx = ix
			ly = iy

			g_dashDuration = PLAYER_DASH_LENGTH
			g_dashReleased = false;
		end
	else
		g_dashReleased = true
	end

	--calculate updated movement
	local maxAcceleration = g_inDash and PLAYER_DASH_ACCELERATION or PLAYER_ACCELERATION
	local maxVelocity = g_inDash and PLAYER_MAX_DASH_VELOCITY or PLAYER_MAX_VELOCITY


	--current speedVelocity and speedAngle
	local smag, cang = cMovement:get_speed(), cMovement:get_angle()
	--calculate input direction
	local tmag, tang = math.to_polar(ix, iy)

	--need to turn compute shortest turn 
	local turn = math.compute_smallest_turn(tang, cang);

	if tmag > 0.1 then
		--Theres a mechanic called quick shift
		--if the player switches direction quickly its instant
		if math.abs(turn) > PLAYER_QUICK_SHIFT_THRESHOLD then
			cMovement:set_angle(tang)
		else
			cMovement:set_angle_change(turn)
		end
	else
		cMovement:set_angle_change(0)
	end

	this:get_component(ComponentMovement):set_speed_cap(maxVelocity)

	--calculate acceleration
	--consider no input case
	if tmag < 0.1 then
		--if speed is slow, set speed and acceleration to zero
		if math.clamp(cMovement:get_speed(), 0, cMovement:get_speed()) <= PLAYER_ACCELERATION then
			cMovement:set_speed(0.001)
			cMovement:set_speed_change(0)
		--otherwise, slow down the player
		else 
			cMovement:set_speed_change(-PLAYER_ACCELERATION)
		end
	--otherwise set acceleration as expected
	else
		cMovement:set_speed_change(maxAcceleration)
	end

end

--Standard bullet handler
g_playerSpawnBullets = function()
	--The position of the player
	local cPosition = this:get_component(ComponentPosition)

	--A component to look at movement
	local cInput = this:get_component(ComponentInput)

	--Timing component
	local cTiming = this:get_component(ComponentTimer)

	--create bullet entities
	if cInput:query_down("shoot") == 1 and 
			(cTiming:get_cycle() % PLAYER_SHOOT_TIMING) == 0 then
		for bAngle in range(45, 136, 15) do
			local cSpawner = this:get_component(ComponentSpawner)
			cSpawner:create_entity(EntityPlayerBullets)
	
			cSpawner:add_component(ComponentPosition.create(0, 40))
			cSpawner:add_component(ComponentCollision.create(100))
			cSpawner:add_component(ComponentDamage.create(10))
			cSpawner:add_component(ComponentOffsetOnce.create())
			cSpawner:add_component(ComponentMinAmbient.create(1.0))
			cSpawner:add_component(ComponentRotation.create())
			cSpawner:add_component(ComponentPointLight.create(0, 0, 0, 0.9647, 0.8039, 0.3751, 0.004, 0.3, 4.85))

			local bulletGComponents = ComponentGraphics.create("bullet_cross")
			bulletGComponents:set_animation(1)
			bulletGComponents:set_scale(1.1)
			cSpawner:add_component(bulletGComponents)

			local bulletMComponent = ComponentMovement.create()
			bulletMComponent:set_speed(16.5)
			bulletMComponent:set_angle(bAngle)
			cSpawner:add_component(bulletMComponent)

	
			cSpawner:push_entity()
		end
	end
end