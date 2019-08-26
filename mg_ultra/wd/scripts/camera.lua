--library control script
--Camera entity (this) has a position and camera component attached
--This script is run asyncronous to rendering

cPosition = this:get_component(ComponentPosition)
cCamera = this:get_component(ComponentCamera)

--cPosition:add_position(1, 1)