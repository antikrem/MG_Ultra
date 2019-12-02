---unified_lighting.lua

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

function a() 
	DirectionalLight.add_new(1.0, 1.0, 1.0, 1.0, 1.0, 1.0)
end