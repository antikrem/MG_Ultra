--background_master.lua
--Manages background master
--"this" references the background master

local backgroundLayers = this:get_component(ComponentMultiEntity)
this:get_component(ComponentMultiEntity):for_each(BackgroundMaster.update_background_layer)