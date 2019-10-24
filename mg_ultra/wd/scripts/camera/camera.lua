--library control script
--Camera entity (this) has a position and camera component attached
--This script is run asyncronous to rendering

--Pull components for the camera entity
camPosition = this:get_component(ComponentPosition)
camCamera = this:get_component(ComponentCamera)

--Get the player entity and pull components
player = EntityPool.get_cached_entity(EntityPlayer)

--set camera x,y position to the player
x,y = player:get_component(ComponentPosition):get_position()
camPosition:set_position(x, y)
camCamera:set_view_target(x, y)