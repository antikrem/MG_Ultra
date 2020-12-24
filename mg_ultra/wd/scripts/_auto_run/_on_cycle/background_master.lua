--background_master.lua
--Manages background master
--"this" references the background master

local bm = EntityPool.get_cached_entity(EntityBackgroundMaster)

if not is_nil(bm) then
	local backgroundLayers = bm:get_component(ComponentMultiEntity)
	bm:get_component(ComponentMultiEntity):for_each(BackgroundMaster.update_background_layer)
end
