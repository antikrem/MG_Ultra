-- \scripts\camera\initialise_camera.lua

-- Executed when a camera entity is created
-- "this" references camera entity

-- Setup camera to be an appropiate fov
-- this is based on parameters in the registar
-- and camera position
-- which can be set in the campaign/level load scripts
local viewSpaceY = registar:get("view_space_y")

-- Set to true for the camera to be updated with camera.lua
registar:add("camera_script_update", false)

if not is_nil(viewSpaceY) then
	local cCamera = this:get_component(ComponentCamera)
	local cPosition = this:get_component(ComponentPosition)
	local _, _, height = cPosition:get_position()
	cCamera:set_fov( 2 *  math.deg(math.atan((viewSpaceY / 2) / (-1 * height))) )
end

Camera = {}

-- Set Clipfar
Camera.set_clip_far = function(clipFar)
	local camera = EntityPool.get_camera()
	if camera then camera:get_component(ComponentCamera):set_clip_far(clipFar) end
end

-- Get Clipfar
Camera.get_clip_far = function()
	local camera = EntityPool.get_camera()
	if camera then return camera:get_component(ComponentCamera):get_clip_far() end
end

-- Set Clipnear
Camera.set_clip_near = function(clipNear)
	local camera = EntityPool.get_camera()
	if camera then camera:get_component(ComponentCamera):set_clip_near(clipNear) end
end

-- Get Clipnear
Camera.get_clip_near = function(clipFar)
	local camera = EntityPool.get_camera()
	if camera then return camera:get_component(ComponentCamera):get_clip_near() end
end

-- Enable camera scripts
Camera.enable = function()
	registar:update("camera_script_update", true)
end

-- Disable camera scripts
Camera.disable = function()
	registar:update("camera_script_update", false)
end

-- Reset camera
Camera.reset = function()
	registar:update("camera_script_update", false)
	local camera = EntityPool.get_camera()
	if camera then 
		camera:get_component(ComponentCamera):set_view_target(0, 0)
		camera:get_component(ComponentPosition):set_position(0, 0)
	end
end