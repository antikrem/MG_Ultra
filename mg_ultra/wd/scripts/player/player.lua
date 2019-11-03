--executed to update player


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

--add has no effect if value is already set
--cEScript:add("test", 10)

--testValue = cEScript:get("test")
--testValue = testValue + 1
--cEScript:set("test", testValue)
--print(testValue)

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

--movement is calculated by treating velocity magnitude and angle seperatly
--angle will always point to input
--magnitude increases linearly to max, and decays quickly with no input

local dmag, dang = to_polar( cMovement:get_velocity() )

--set magnitude and angle depending on input
if (to_magnitude(ix,iy) < 0.1) then
	dmag = dmag - PLAYER_ACCELERATION
else
	dmag = dmag + PLAYER_ACCELERATION
	__, dang = to_polar(ix, iy)
end
dmag = clamp(dmag, 0, PLAYER_MAX_VELOCITY)

cMovement:set_velocity( to_point(dmag, dang) )

--create a bullet entity
if cInput:query_down("shoot") == 1 and 
		(cTiming:get_cycle() % PLAYER_SHOOT_TIMING) == 0 then
    x, y = cPosition:get_position()
	for bAngle in range(-45, 46, 15) do
		cSpawner = this:get_component(ComponentSpawner)
		cSpawner:create_entity(6)
	
		cSpawner:add_component(ComponentPosition.create(x, y))

		local bulletGComponents = ComponentGraphics.create("default")
		bulletGComponents:set_animation(2)
		bulletGComponents:set_scale(0.1)
		cSpawner:add_component(bulletGComponents)

		local bulletMComponent = ComponentMovement.create()
		bulletMComponent:set_speed(10.0)
		bulletMComponent:set_angle(bAngle)
		bulletMComponent:set_rotation_speed(5)
		cSpawner:add_component(bulletMComponent)
	
		cSpawner:push_entity()
	end
end
