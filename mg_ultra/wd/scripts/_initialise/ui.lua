--\scripts\_initialise\ui.lua

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

UI.display_title_card = function(titleSet, fadeIn, delay, fadeOut)
	local e = Entity.create(EntityGeneric)
	e:add_component(ComponentPosition.create(0,0,-1))
	e:add_component(ComponentTransparency.create(1.0, 1.0 / fadeIn))

	local gra = ComponentGraphics.create(titleSet)
	gra:set_render_in_3D(false)
	e:add_component(gra)
	
	

	EntityPool.add_entity(e)


end