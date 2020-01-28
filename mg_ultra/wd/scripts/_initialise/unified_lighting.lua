---\scripts\_initialise\unified_lighting.lua

--Global accessor for controlling ambient light
AmbientLight = {}

AmbientLight.set_strength = setAmbientLightStrength;

AmbientLight.get_strength = getAmbientLightStrength;

AmbientLight.set_colour = setAmbientLightColour;

AmbientLight.get_colour = getAmbientLightColour;

--Accessor for controlling directional lights
DirectionalLight = {}

--Adds a new light to the scene
--x,y,z refers to the position of the light
--in relation to origin, i.e shining from (x,y,z) to (0,0,0)
DirectionalLight.add_new = function(x, y, z, r, g, b)
	r = r or 1.0
	g = g or 1.0
	b = b or 1.0

	AnonymousSpawner.create_entity(EntityDirectionalLight)
	local cDirectionalLight = ComponentDirectionalLight.create(x, y, z, r, g, b)
	AnonymousSpawner.add_component(cDirectionalLight)
	AnonymousSpawner.push_entity()
end

--Interface for handling fog parameters
Fog = {}

--set fog colour
Fog.set_colour = setFogColour

--get fog colour
Fog.get_colour = getFogColour

--set fog exponential growth factor
Fog.set_strength = setFogStrength

--get fog exponential growth factor
Fog.get_strength = getFogStrength

--set fog starting depth
Fog.set_starting_depth = setFogStart

--get fog starting distance
Fog.get_starting_depth = getFogStart

--sets fog starting depth
Fog.set_max = setFogMax

--get fog starting distance
Fog.set_max = getFogMax

--Colours modulation
ColourModulation = {}

--Sets strength of both frontground and background
ColourModulation.set_strength = function(current, rate, target)
	rate = rate or 1.0
	if is_nil(target) then target = current end
	
	setColourModulationStrengths(current, rate, target)

end

--Sets colour of foreground modulation
ColourModulation.set_foreground_colour = setColourModulationForegroundColour

--gets colour of foreground modulation
ColourModulation.get_foreground_colour = getColourModulationForegroundColour

--Sets colour of background modulation
ColourModulation.set_background_colour = setColourModulationBackgroundColour

--gets colour of background modulation
ColourModulation.get_background_colour = getColourModulationBackgroundColour

--sets foreground cutoff
ColourModulation.set_foreground_cutoff = setColourModulationForegroundCutoff

--sets background cutoff
ColourModulation.set_background_cutoff = setColourModulationBackgroundCutoff

--sets cutoff to be right next to each other at distance
ColourModulation.set_cutoff = function(distance)
	ColourModulation.set_foreground_cutoff(distance)
	ColourModulation.set_background_cutoff(distance + 0.01)

end