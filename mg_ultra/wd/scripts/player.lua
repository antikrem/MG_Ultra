--executed to update player

--The position of the player
cPosition = this:get_component(ComponentPosition)
--More sophisticated interface for controlling movement
--A component to look at movement
cInput = this:get_component(ComponentInput)

--vector representing movement input
x,y = 0,0

if cInput:query_down("left") == 1 then
	x = x - 1;
end
if cInput:query_down("right") == 1 then
	x = x + 1;
end


cPosition:add_position(x, y)