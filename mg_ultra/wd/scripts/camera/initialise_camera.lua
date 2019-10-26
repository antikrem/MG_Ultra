--Executed when a camera entity is created
--"this" references camera entity

--setup camera to be an appropiate fov
--this is based on parameters in the registar
--and camera position
--which can be set in the campaign/level load scripts
local viewSpaceY = registar:get("view_space_y")


if not is_nil(viewSpaceY) then
	local cCamera = this:get_component(ComponentCamera)
	local cPosition = this:get_component(ComponentPosition)
	local _, _, height = cPosition:get_position()
	cCamera:set_fov( 2 *  math.deg(math.atan((viewSpaceY / 2) / height)) )

end