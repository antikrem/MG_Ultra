--library control script
--Camera entity (this) has a position and camera component attached
--This script is run asyncronous to rendering

--Pull components for the camera entity
local camPosition = this:get_component(ComponentPosition)
local camCamera = this:get_component(ComponentCamera)

--Get the player entity and pull components
local player = EntityPool.get_cached_entity(EntityPlayer)

--if theres a player, then base camera on its position
if not is_nil(player) then
    --set camera x,y position to the player
    local x, y = player:get_component(ComponentPosition):get_position()
    camPosition:set_position(x, 0)
    camCamera:set_view_target(x, 0)
end