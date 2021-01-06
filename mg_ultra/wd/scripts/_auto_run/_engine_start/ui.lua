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

-- Get a ui item from UI master by Namespace
UI._get_item = function(name)
	return EntityPool.get_cached_entity(EntityUIManager):get_component(ComponentMultiEntity):get(name)
end

-- Starts UI
UI.start_ui = function() 
	-- Single entity that manages all other entities
	local master = Entity.create(EntityUIManager)
	master:add_component(ComponentMultiEntity.create())
	local manager = ComponentSpawner.create()
	master:add_component(manager)
	EntityPool.add_cached_entity(master)


	local e = Entity.create(EntityUIElement)
	e:add_component(ComponentName.create("ScoreBoard"))
	e:add_component(ComponentPosition.create(-900, 500))
	local c = ComponentText.create("text_consolas58")
	c:set_render_in_3D(false)
	c:set_text("000000000")
	c:set_visible(false)
	e:add_component(c)

	manager:add_entity(e)

	local e = Entity.create(EntityUIElement)
	e:add_component(ComponentName.create("FragmentBoard"))
	e:add_component(ComponentPosition.create(-900, 450))
	local c = ComponentText.create("text_consolas58")
	c:set_render_in_3D(false)
	c:set_text(tostring(g_fragments) .. "/" .. tostring(g_nextFragments))
	c:set_visible(false)
	e:add_component(c)

	manager:add_entity(e)

	local e = Entity.create(EntityUIElement)
	e:add_component(ComponentName.create("PowerBoard"))
	e:add_component(ComponentPosition.create(-900, 400))
	local c = ComponentText.create("text_consolas58")
	c:set_render_in_3D(false)
	c:set_text("00.00%")
	c:set_visible(false)
	e:add_component(c)

	manager:add_entity(e)

	UI.create_panes(manager)

	UI.pane_hidden_value = 0
	UI.uiCreated = true
end

UI.show_ui = function()
	UI._get_item("ScoreBoard"):get_component(ComponentText):set_visible(true)

	UI._get_item("FragmentBoard"):get_component(ComponentText):set_visible(true)

	UI._get_item("PowerBoard"):get_component(ComponentText):set_visible(true)

	UI.uiVisible = true
end

UI.hide_ui = function()
	UI._get_item("ScoreBoard"):get_component(ComponentText):set_visible(false)

	UI._get_item("FragmentBoard"):get_component(ComponentText):set_visible(false)

	UI._get_item("PowerBoard"):get_component(ComponentText):set_visible(false)

	UI.uiVisible = false
end

UI.update_ui = function()
	UI._get_item("ScoreBoard"):get_component(ComponentText):set_text(string.pad_string(tostring(g_points), 9, "0", true))

	UI._get_item("FragmentBoard"):get_component(ComponentText):set_text(tostring(g_fragments) .. "/" .. tostring(g_nextFragments))

	UI._get_item("PowerBoard"):get_component(ComponentText):set_text(tostring(g_power) .. "%")

	UI.pane_update()
end

UI.PANEL_WIDTH = 240
UI.SHOW_X_POSITION = 840
UI.HIDDEN_X_POSITION = UI.PANEL_WIDTH + UI.SHOW_X_POSITION
UI.PANEL_TRANSPARENCY = 0.7


-- Ranged between 0 (show) and 1 (hidden)
UI.pane_hidden_value = 1.0
UI.pane_hidden_target = 1.0
UI.pane_hidden_rate = 1.0

-- Draws left and right panes
UI.create_panes = function(manager) 
	local leftPane = Entity.create(EntityUIElement)
	leftPane:add_component(ComponentName.create("LeftPane"))
	leftPane:add_component(ComponentPosition.create(-UI.SHOW_X_POSITION, 0, 10))
	leftPane:add_component(ComponentNoBoundsControl.create())
	local lc = ComponentGraphics.create("ui_left_pane")
	lc:set_render_in_3D(false)
	lc:set_transparency(UI.PANEL_TRANSPARENCY)
	leftPane:add_component(lc)
	manager:add_entity(leftPane)

	local rightPane = Entity.create(EntityUIElement)
	rightPane:add_component(ComponentName.create("RightPane"))
	rightPane:add_component(ComponentPosition.create(UI.SHOW_X_POSITION, 0, 10))
	rightPane:add_component(ComponentNoBoundsControl.create())
	local rc = ComponentGraphics.create("ui_right_pane")
	rc:set_render_in_3D(false)
	rc:set_transparency(UI.PANEL_TRANSPARENCY)
	rightPane:add_component(rc)
	manager:add_entity(rightPane)
	
	UI.pane_update()
end

UI.pane_update = function()
	UI.pane_hidden_value = math.tend_to(UI.pane_hidden_value, UI.pane_hidden_target, UI.pane_hidden_rate)

	local leftPane = UI._get_item("LeftPane")
	if is_not_nil(leftPane) then
		local pos = leftPane:get_component(ComponentPosition)
		pos:set_position(-UI.SHOW_X_POSITION - UI.pane_hidden_value * UI.PANEL_WIDTH, 0, 10)
	end

	local rightPane = UI._get_item("RightPane")
	if is_not_nil(rightPane) then
		local pos = rightPane:get_component(ComponentPosition)
		pos:set_position(UI.SHOW_X_POSITION + UI.pane_hidden_value * UI.PANEL_WIDTH, 0, 10)
	end
end

UI.hide_panes_now = function() 
	UI.pane_hidden_value = 1
	UI.pane_update()
end

UI.show_panes = function(cycles)
	if is_nil(cycles) then cycles = 100 end

	UI.pane_hidden_target = 0.0
	UI.pane_hidden_rate = 1.0/cycles
end


UI.hide_panes = function(cycles)
	if is_nil(cycles) then cycles = 100 end

	UI.pane_hidden_target = 1.0
	UI.pane_hidden_rate = 1.0/cycles
end