-- scripts\initialise_player.lua

--Registar value to control
GlobalRegistar.add("player_alive", true)

-- Player movement

--Table used for gettings certain velocity values
PLAYER_MAX_VELOCITY_TABLE = {
	DEFAULT = 13,
	DASH = 36,
	FOCUS = 5.8
}

--Similar table for acceleration
PLAYER_MAX_ACCELERATION_TABLE = {
	DEFAULT = 1.5,
	DASH = 36,
	FOCUS = 0.5
}

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
PLAYER_SHOOT_TIMING = 11

-- Variables used to control player movement
this:get_component(ComponentMovement):set_speed_cap(PLAYER_MAX_VELOCITY_TABLE["DEFAULT"])
this:get_component(ComponentMovement):set_angle_cap(PLAYER_MAX_TURN_VELOCITY)

-- Colour of player light
-- The light is activated by other systems (mostly weather)
this:get_component(ComponentPointLight):set_colour(0.9647, 0.8039, 0.3751)

-- Counter for number of dead frames
g_sequentialDeadFrames = 0

--Flip for player
g_playerBulletOscillator = -1

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

--loading assets for player
Audio.request_load_file("player_shoot_tick", "shoot_click.wav")
Audio.flush_queue()

--set this audio component to play shoot tick
this:get_component(ComponentAudio):set_source("player_shoot_tick")
this:get_component(ComponentAudio):set_repeat(true)


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

	local isDashInput = cInput:query_down("dash") == 1 

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
	if isDashInput then
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

	local isFocusInput = cInput:query_down("focus") == 1 and not g_inDash

	local movementMode = g_inDash and "DASH" or (isFocusInput and "FOCUS" or "DEFAULT")

	--calculate updated movement
	local maxAcceleration = PLAYER_MAX_ACCELERATION_TABLE[movementMode]
	local maxVelocity = PLAYER_MAX_VELOCITY_TABLE[movementMode]

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
		if math.clamp(cMovement:get_speed(), 0, cMovement:get_speed()) 
				<= PLAYER_MAX_ACCELERATION_TABLE[movementMode] then
			cMovement:set_speed(0.001)
			cMovement:set_speed_change(0)
		--otherwise, slow down the player
		else 
			cMovement:set_speed_change(-PLAYER_MAX_ACCELERATION_TABLE[movementMode])
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

	g_playerBulletOscillator = g_playerBulletOscillator * -1

	--create bullet entities
	if cInput:query_down("shoot") == 1 and 
			(cTiming:get_cycle() % PLAYER_SHOOT_TIMING) == 0 then
		for bAngle in range(55, 126, 10) do
			local cSpawner = this:get_component(ComponentSpawner)
			cSpawner:create_entity(EntityPlayerBullets)
	
			cSpawner:add_component(ComponentPosition.create(0, 40, LAYER_PLAYER_BULLETS))
			cSpawner:add_component(ComponentCollision.create(100))
			cSpawner:add_component(ComponentDamage.create(10))
			cSpawner:add_component(ComponentOffsetOnce.create())
			cSpawner:add_component(ComponentMinAmbient.create(0.55))
			cSpawner:add_component(ComponentRotation.create())
			cSpawner:add_component(ComponentPointLight.create(1.0, 0.75, 0.05, 0.0015, 0.07, 3.2))

			local bulletGComponents = ComponentGraphics.create("bullet_cross")
			bulletGComponents:set_animation(1)
			--bulletGComponents:set_scale(0.95)
			cSpawner:add_component(bulletGComponents)

			local bulletMComponent = ComponentMovement.create()
			bulletMComponent:set_speed(27)
			bulletMComponent:set_angle(bAngle + 2.5 * g_playerBulletOscillator)
			cSpawner:add_component(bulletMComponent)

	
			cSpawner:push_entity()
		end
	end

	-- If we get a press even, play the audio source
	if cInput:query_press("shoot") == 1 then
		this:get_component(ComponentAudio):set_repeat(true)
		this:get_component(ComponentAudio):play_source()
	-- Otherwise disable repeat to stop audio naturally
	elseif cInput:query_down("shoot") == 0 then
		this:get_component(ComponentAudio):set_repeat(false)
	end

end

--Handler function when player is hit
g_bulletPlayerCollision = function() 
	if GlobalRegistar.get("player_alive") then
		this:get_component(ComponentClampPosition):set_active(false)
		g_sequentialDeadFrames = 0
		GlobalRegistar.update("player_alive", false)
	end
end