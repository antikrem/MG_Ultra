--scripts\playerplayer.lua
--executed regularly to update player

--The position of the player
local cPosition = this:get_component(ComponentPosition)
--More sophisticated interface for controlling movement
local cMovement = this:get_component(ComponentMovement)
--Get graphics
local cGraphics  = this:get_component(ComponentGraphics)
--A component to look at movement
local cInput = this:get_component(ComponentInput)
--Extended scripting functionality
local cEScript = this:get_component(ComponentExtendedScripting)
--Timing component
local cTiming = this:get_component(ComponentTimer)

--represents input
local ix, iy = 0,0

if cInput:query_down("left") == 1 then
	ix = ix - 1;
end
if cInput:query_down("right") == 1 then
	ix = ix + 1;
end
if cInput:query_down("up") == 1 then
	iy = iy + 1;
end
if cInput:query_down("down") == 1 then
	iy = iy - 1;
end

--Added dash variables
if g_dashCooldown > 0 then
	g_dashCooldown = g_dashCooldown - 1
end
if g_dashDuration > 0 then
	g_dashDuration = g_dashDuration - 1
end

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

--create a bullet entity
if cInput:query_down("shoot") == 1 and 
		(cTiming:get_cycle() % PLAYER_SHOOT_TIMING) == 0 then
	for bAngle in range(45, 136, 15) do
		cSpawner = this:get_component(ComponentSpawner)
		cSpawner:create_entity(EntityPlayerBullets)
	
		cSpawner:add_component(ComponentPosition.create(0, 40))
		cSpawner:add_component(ComponentCollision.create(100))
		cSpawner:add_component(ComponentDamage.create(10))
		cSpawner:add_component(ComponentOffsetOnce.create())
		cSpawner:add_component(ComponentMinAmbient.create(1.0))

		local bulletGComponents = ComponentGraphics.create("player")
		bulletGComponents:set_animation(1)
		bulletGComponents:set_scale(0.1)
		cSpawner:add_component(bulletGComponents)

		local bulletMComponent = ComponentMovement.create()
		bulletMComponent:set_speed(15.0)
		bulletMComponent:set_angle(bAngle)
		bulletMComponent:set_rotation_speed(5)
		cSpawner:add_component(bulletMComponent)

	
		cSpawner:push_entity()
	end
end