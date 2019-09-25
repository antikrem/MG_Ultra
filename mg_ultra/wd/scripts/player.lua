--executed to update player

--some constant values
WIDE_WIDTH = 1520

--Player movement
PLAYER_ACCELERATION = 0.25
PLAYER_MAX_VELOCITY = 5.0
PLAYER_MAX_FOCUS_VELOCITY = 2.0
PLAYER_MAX_DASH_VELOCITY = 15.0

--The position of the player
cPosition = this:get_component(ComponentPosition)
--More sophisticated interface for controlling movement
cMovement = this:get_component(ComponentMovement)
--A component to look at movement
cInput = this:get_component(ComponentInput)
--Extended scripting functionality
cEScript = this:get_component(ComponentExtendedScripting)

--add has no effect if value is already set
--cEScript:add("test", 10)

--testValue = cEScript:get("test")
--testValue = testValue + 1
--cEScript:set("test", testValue)
--print(testValue)

--represents input
ix, iy = 0,0

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

dmag, dang = to_polar( cMovement:get_velocity() )

--set magnitude and angle depending on input
if ( to_magnitude(ix,iy) < 0.1) then
	dmag = dmag - PLAYER_ACCELERATION
else
	dmag = dmag + PLAYER_ACCELERATION
	__, dang = to_polar(ix, iy)
end
dmag = clamp(dmag, 0, PLAYER_MAX_VELOCITY)

cMovement:set_velocity( to_point(dmag, dang) )


cPosition:set_position( cMovement:get_updated_position(cPosition:get_position()) )