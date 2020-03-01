-- \scripts\_auto_run\_engine_start\ui.lua

-- Namespace for ui modification
UI = {}

UI.set_border = function(value, rate)
	if is_nil(rate) then
		setBorderValue(value)
	else
		setBorderTrendValue(value, rate)
	end
end

UI.apply_cinematic_borders = function()
	UI.set_border(0.76, 0.009)
end

UI.remove_cinematic_borders = function()
	UI.set_border(1.0, 0.009)
end

UI.display_title_card = function(titleSet, fadeIn, delay, fadeOut, scale)

	if is_nil(scale) then scale = 1 end
	local e = Entity.create(EntityGeneric)
	e:add_component(ComponentPosition.create(0,0,-20))

	if fadeIn < 1 then fadeIn = 1 end
	e:add_component(ComponentTransparency.create(1.0, 1.0 / fadeIn))

	local gra = ComponentGraphics.create(titleSet)
	gra:set_scale(scale)
	e:add_component(gra)
	
	local tim = ComponentTimer.create()
	tim:add_callback(
		delay, 
		"this:get_component(ComponentTransparency):set_rate(" .. tostring(1.0 / fadeOut) ..")"..
		"this:get_component(ComponentTransparency):set_target(0)" 
		
	)
	tim:add_callback(
		delay + fadeOut, 
		"this:kill()" 

	)
	e:add_component(tim)

	EntityPool.add_entity(e)


end

-- False when ui hasn't spawned yet
UI.uiCreated = false

-- True when ui is being drawn
UI.uiVisible = false

UI.start_ui = function() 
	local e = Entity.create(EntityScoreBoard)
	e:add_component(ComponentPosition.create(-900, 500))
	local c = ComponentText.create("text_consolas58")
	c:set_render_in_3D(false)
	c:set_text("000000000")
	c:set_visible(false)
	e:add_component(c)

	EntityPool.add_cached_entity(e)

	local e = Entity.create(EntityFragmentBoard)
	e:add_component(ComponentPosition.create(-900, 450))
	local c = ComponentText.create("text_consolas58")
	c:set_render_in_3D(false)
	c:set_text(tostring(g_fragments) .. "/" .. tostring(g_nextFragments))
	c:set_visible(false)
	e:add_component(c)

	EntityPool.add_cached_entity(e)

	local e = Entity.create(EntityPowerBoard)
	e:add_component(ComponentPosition.create(-900, 400))
	local c = ComponentText.create("text_consolas58")
	c:set_render_in_3D(false)
	c:set_text("00.00%")
	c:set_visible(false)
	e:add_component(c)

	EntityPool.add_cached_entity(e)

	UI.uiCreated = true
end

UI.show_ui = function()
	local e = EntityPool.get_cached_entity(EntityScoreBoard)
	e:get_component(ComponentText):set_visible(true)

	local e = EntityPool.get_cached_entity(EntityFragmentBoard)
	e:get_component(ComponentText):set_visible(true)

	local e = EntityPool.get_cached_entity(EntityPowerBoard)
	e:get_component(ComponentText):set_visible(true)

	UI.uiVisible = true
end

UI.hide_ui = function()
	local e = EntityPool.get_cached_entity(EntityScoreBoard)
	e:get_component(ComponentText):set_visible(false)

	local e = EntityPool.get_cached_entity(EntityFragmentBoard)
	e:get_component(ComponentText):set_visible(false)

	local e = EntityPool.get_cached_entity(EntityPowerBoard)
	e:get_component(ComponentText):set_visible(false)

	UI.uiVisible = false
end

UI.update_ui = function()
	local e = EntityPool.get_cached_entity(EntityScoreBoard)
	e:get_component(ComponentText):set_text(string.pad_string(tostring(g_points), 9, "0", true))

	local e = EntityPool.get_cached_entity(EntityFragmentBoard)
	e:get_component(ComponentText):set_text(tostring(g_fragments) .. "/" .. tostring(g_nextFragments))

	local e = EntityPool.get_cached_entity(EntityPowerBoard)
	e:get_component(ComponentText):set_text(tostring(g_power) .. "%")
end