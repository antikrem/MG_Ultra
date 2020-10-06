-- scripts\initialise_player_system.lua

--Registar value to control
GlobalRegistar.add("player_alive", true)
GlobalRegistar.add("player_active", false)

-- Player movement

--Table used for gettings certain velocity values
PLAYER_MAX_VELOCITY_TABLE = {
	DEFAULT = 11.5,
	DASH = 50,
	FOCUS = 4.3
}

--Similar table for acceleration
PLAYER_MAX_ACCELERATION_TABLE = {
	DEFAULT = 1.5,
	DASH = 50,
	FOCUS = 0.5
}

-- Additional soft position clamping
PLAYER_X_CLAMP = 960
PLAYER_Y_CLAMP = 540

-- Dash related variables
PLAYER_DASH_LENGTH = 13
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
PLAYER_SHOOT_TIMING = 10

-- Counter for number of dead frames
g_sequentialDeadFrames = 0

--Flip for player
g_playerBulletOscillator = -1

-- Values that represent the last dash direction
g_lx, g_ly = 0,0
-- Set to true when a dash is occuring
g_inDash = false
-- Set to true when focus
g_inFocus = false
-- Set to true when the dash key is released
g_dashReleased = true
-- Time spent in dash
g_dashDuration = 0
-- Time spent in cooldown
g_dashCooldown = 0

-- Tends from 0 to one when in shift
g_shiftFactor = 0
-- Shift factor change value
PLAYER_SHIFT_FACTOR_DELTA = 0.02

-- Variables for power 
PLAYER_POWER_LEVEL_THRESHOLD = 20

-- Constants for bullet friend circle position
PLAYER_FRIEND_RESTING_OFFSET = 150
PLAYER_FRIEND_FOCUS_OFFSET = 150
PLAYER_FRIEND_FOCUS_ROTATION_MULTIPLIER = 17

-- Constants for bullet targets when firing
PLAYER_FRIEND_TARGET = 550

-- Variables about health
g_lives = 3

-- Total point
g_points = 0

-- Variables on fragments for new lives
FRAGMENT_PERIOD = 25
g_fragments = 0
g_nextFragments = FRAGMENT_PERIOD

-- Power of player
PLAYER_RESTING_FRAGMENT_POINTS_DRAIN = 0.05
PLAYER_FOCUS_FRAGMENT_POINTS_DRAIN = 0.01
g_power = 0
g_power_level = 0

-- Clear Radius
PLAYER_CLEAR_RADIUS = 250
PLAYER_CLEAR_FRAMES = 250

--loading assets for player
Audio.request_load_file("player_shoot_tick", "shoot_click.wav")
Audio.flush_queue()

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

	local focusValue = cInput:query_down("focus")
	g_inFocus = focusValue == 1 and not g_inDash

	local movementMode = g_inDash and "DASH" or (g_inFocus and "FOCUS" or "DEFAULT")

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

	if g_inDash then
		this:get_component(ComponentParticle):spawn(1)	
	end

	-- Modify shift factor
	g_shiftFactor = math.tend_to(g_shiftFactor, focusValue, PLAYER_SHIFT_FACTOR_DELTA)
end

-- spawn bullet at given layer
g_playerSpawnBullet = function(x, y, tx, ty)
	--The position of the player
	local cPosition = this:get_component(ComponentPosition)

	local angle = math.angle_to(x, y, tx, ty)

	local cSpawner = this:get_component(ComponentSpawner)
	cSpawner:create_entity(EntityPlayerBullets)
			
	cSpawner:add_component(ComponentPosition.create(x, y, LAYER_PLAYER_BULLETS))
	cSpawner:add_component(ComponentCollision.create(27))
	cSpawner:add_component(ComponentDamage.create(10))
	cSpawner:add_component(ComponentOffsetOnce.create())
	cSpawner:add_component(ComponentMinAmbient.create(0.8))
	cSpawner:add_component(ComponentRotation.create())
	cSpawner:add_component(ComponentPointLight.create(1.0, 0.75, 0.05, 0.0015, 0.07, 3.2))

	local pod = ComponentParticleOnDeath.create("death_gold_fragments")
	pod:set_velocity_range(1.0, 1.3);
	pod:set_direction_deviation(10);
	cSpawner:add_component(pod)
			
	local bulletGComponents = ComponentGraphics.create("bullet_cross")
	bulletGComponents:set_animation(1)
	bulletGComponents:set_scale(0.7)
	cSpawner:add_component(bulletGComponents)
			
	local bulletMComponent = ComponentMovement.create()
	bulletMComponent:set_speed(27)
	bulletMComponent:set_angle(angle + 2.5 * g_playerBulletOscillator)
	cSpawner:add_component(bulletMComponent)

	cSpawner:push_entity()

end


-- Standard bullet handler
g_playerSpawnBullets = function()

	--A component to look at movement
	local cInput = this:get_component(ComponentInput)

	--Timing component
	local cTiming = this:get_component(ComponentTimer)


	--create bullet entities
	if cInput:query_down("shoot") == 1 and 
			(cTiming:get_cycle() % PLAYER_SHOOT_TIMING) == 0 then

		-- Oscilate bullet position
		g_playerBulletOscillator = g_playerBulletOscillator * -1

		-- Generate target position
		local ty = math.lerp(g_shiftFactor, PLAYER_FRIEND_RESTING_OFFSET, PLAYER_FRIEND_TARGET)

		for i in range(1, g_power_level + 1) do

			-- Generate desired point
			local x, y = math.rotate_point(0, -PLAYER_FRIEND_FOCUS_OFFSET, PLAYER_FRIEND_FOCUS_ROTATION_MULTIPLIER * (i - 0.5))

			-- lerp current position to tarrget
			x = math.lerp(math.max(g_shiftFactor, 0.01), 0, x)
			y = math.lerp(math.max(g_shiftFactor, 0.01), PLAYER_FRIEND_RESTING_OFFSET, y)

			g_playerSpawnBullet(x, y, 0, ty)
			
			-- Generate desired point
			local x, y = math.rotate_point(0, -PLAYER_FRIEND_FOCUS_OFFSET, -PLAYER_FRIEND_FOCUS_ROTATION_MULTIPLIER * (i - 0.5))

			-- lerp current position to tarrget
			x = math.lerp(math.max(g_shiftFactor, 0.01), 0, x)
			y = math.lerp(math.max(g_shiftFactor, 0.01), PLAYER_FRIEND_RESTING_OFFSET, y)

			g_playerSpawnBullet(x, y, 0, ty)
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

-- Cyclic update to modify Power
g_playerPowerUpdate = function()
	local drain = 0
	if g_inFocus then drain = PLAYER_FOCUS_FRAGMENT_POINTS_DRAIN else drain = PLAYER_RESTING_FRAGMENT_POINTS_DRAIN end
	g_power = g_power - drain
	g_power = math.max(0, g_power)
end

-- Handler function when player is hit
g_bulletPlayerCollision = function() 
	if GlobalRegistar.get("player_alive") then
		g_power_level = 1
		this:get_component(ComponentMultiEntity):kill_children()
		this:get_component(ComponentClampPosition):set_active(false)
		g_sequentialDeadFrames = 0
		GlobalRegistar.update("player_alive", false)

		Player.add_clear_aura()
	end
end

Player = {}

-- Enables the player system
Player.enable_player = function()
	GlobalRegistar.add("player_active", true)
end

-- Execute a function that treats player as this
Player.execute_against = function(f, ...)
	this = EntityPool.get_player()

	if is_nil(this) then return end

	f(...)
	this = nil
end

-- Returns player position
Player.get_position = function()
	local player = EntityPool.get_player()
	if player then return player:get_component(ComponentPosition):get_position() else return nil end
end

-- Add friend magic circle
-- Works only when this points to player
Player.add_friend_magic_circle = function(layer)
	-- Create freind magic circles
	local mc = Entity.create(EntityGeneric)

	mc:add_component(ComponentPosition.create(0, PLAYER_FRIEND_RESTING_OFFSET, 0))
	mc:add_component(ComponentGraphics.create("mc_fire_circle"))
	mc:add_component(ComponentRotation.create(0, 2))

	mc:add_component(ComponentOffsetMaster.create(true))
	mc:add_component(ComponentDieWithMaster.create())
	mc:add_component(ComponentNoBoundsControl.create())

	mc:add_component(ComponentPointLight.create(1.0, 0.75, 0.05, 0.0015, 0.07, 3.2))
	mc:add_component(ComponentMinAmbient.create(1.1))

	mc:add_component(ComponentTransparency.create(1.0, 0.005))

	mc:add_component(ComponentName.create(tostring(layer)))

	this:get_component(ComponentSpawner):add_entity(mc)

	mc = Entity.create(EntityGeneric)

	mc:add_component(ComponentPosition.create(0, PLAYER_FRIEND_RESTING_OFFSET, 0))
	mc:add_component(ComponentGraphics.create("mc_fire_circle"))
	mc:add_component(ComponentRotation.create(0, 2))

	mc:add_component(ComponentOffsetMaster.create(true))
	mc:add_component(ComponentDieWithMaster.create())
	mc:add_component(ComponentNoBoundsControl.create())

	mc:add_component(ComponentPointLight.create(1.0, 0.75, 0.05, 0.0015, 0.07, 3.2))
	mc:add_component(ComponentMinAmbient.create(1.1))

	mc:add_component(ComponentTransparency.create(1.0, 0.005))

	mc:add_component(ComponentName.create("-" .. tostring(layer)))

	this:get_component(ComponentSpawner):add_entity(mc)
end


-- Update friend magic circle
-- Works only when this points to player
Player.update_a_friend_magic_circle = function(layer)
	local offsetX, offsetY = this:get_component(ComponentPosition):get_position()


	-- Left circle
	local mc = this:get_component(ComponentMultiEntity):get("-" .. tostring(layer))

	-- Generate desired point
	local targetX, targetY = math.rotate_point(0, -PLAYER_FRIEND_FOCUS_OFFSET, PLAYER_FRIEND_FOCUS_ROTATION_MULTIPLIER * (layer - 0.5))

	-- lerp current position to tarrget
	targetX = math.lerp(g_shiftFactor, 0, targetX)
	targetY = math.lerp(g_shiftFactor, PLAYER_FRIEND_RESTING_OFFSET, targetY)

	if not is_nil(mc) then
		mc:get_component(ComponentPosition):set_position(offsetX + targetX, offsetY + targetY)
	end

	-- Right circle
	local mc = this:get_component(ComponentMultiEntity):get(tostring(layer))

	-- Generate desired point
	local targetX, targetY = math.rotate_point(0, -PLAYER_FRIEND_FOCUS_OFFSET, -PLAYER_FRIEND_FOCUS_ROTATION_MULTIPLIER * (layer - 0.5))

	-- lerp current position to tarrget
	targetX = math.lerp(g_shiftFactor, 0, targetX)
	targetY = math.lerp(g_shiftFactor, PLAYER_FRIEND_RESTING_OFFSET, targetY)

	if not is_nil(mc) then
		mc:get_component(ComponentPosition):set_position(offsetX + targetX, offsetY + targetY)
	end

end

-- Removes friend circles
-- Works only when this points to player
Player.remove_a_friend_magic_circle = function(layer)

	-- Left circle
	local mc = this:get_component(ComponentMultiEntity):get("-" .. tostring(layer))

	if not is_nil(mc) then
		mc:kill()
	end

	-- Right circle
	local mc = this:get_component(ComponentMultiEntity):get(tostring(layer))

	if not is_nil(mc) then
		mc:kill()
	end

end

-- General handling for friends
Player.friend_handle = function(layer)
	requiredLevel = math.floor(g_power / PLAYER_POWER_LEVEL_THRESHOLD) + 1

	while g_power_level < requiredLevel do
		g_power_level = g_power_level + 1
		Player.add_friend_magic_circle(tostring(g_power_level))
	end

	while g_power_level > requiredLevel do
		Player.remove_a_friend_magic_circle(tostring(g_power_level))
		g_power_level = g_power_level - 1
	end

	for i in range(1, g_power_level + 1) do
		Player.update_a_friend_magic_circle(i)
	end
end

-- Add hitbox child
Player.add_hitbox = function()
	-- Create freind magic circles
	local mc = Entity.create(EntityGeneric)

	mc:add_component(ComponentPosition.create(0, 0, LAYER_PLAYER - 5))
	mc:add_component(ComponentGraphics.create("player_hitbox"))

	mc:add_component(ComponentOffsetMaster.create(true))
	mc:add_component(ComponentDieWithMaster.create())
	mc:add_component(ComponentNoBoundsControl.create())

	mc:add_component(ComponentMinAmbient.create(1))

	mc:add_component(ComponentTransparency.create(0.0, 0.005))

	mc:add_component(ComponentName.create("hitbox"))

	this:get_component(ComponentSpawner):add_entity(mc)

end

-- Attaches a clearing entitity
Player.add_clear_aura = function()
	-- Create timed death emmitter
	local mc = Entity.create(EntityBulletClearer)

	mc:add_component(ComponentPosition.create(0, 0, LAYER_PLAYER))

	mc:add_component(ComponentOffsetMaster.create(true))
	mc:add_component(ComponentDieWithMaster.create())
	mc:add_component(ComponentNoBoundsControl.create())

	mc:add_component(ComponentDeathEmitter.create())
	mc:add_component(ComponentCollision.create(250))

	mc:add_component(ComponentName.create("clearer"))

	local timer = ComponentTimer.create()
	timer:add_callback(
		PLAYER_CLEAR_FRAMES, 
		"this:kill()" 
	)

	mc:add_component(timer)

	this:get_component(ComponentSpawner):add_entity(mc)

end