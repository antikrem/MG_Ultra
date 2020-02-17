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