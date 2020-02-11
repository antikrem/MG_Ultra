---Extended library
--Classes and mg functionality have been bounded

--Add a function within Entity class to allow components to be retrived 
--by component name, returns nil if the entity does not have the specified component
function Entity.get_component(self, component)
	local ptr = self:getComponent(component.type())
	if ptr == nil then
		return nil
	else
		return component.cast(ptr)
	end
end

--Adding a component to an entity after its been put in a pool
--will not attach them for modification from associated systems
--will return false if operation failed
function Entity.add_component(self, component)
	return self:addComponent(component.type(), component)
end

--Function to get angle from a position to player
--Otherwise just angles down
function get_angle_to_player(x, y)
	
	local p = EntityPool.get_cached_entity(EntityPlayer)
	if is_nil(p) then return -90 end

	local p_x, p_y, angle
	p_x, p_y = p:get_component(ComponentPosition):get_position()

	_, angle = math.to_polar(p_x - x, p_y - y)
	return angle
end

--Creates a sprite and attaches it to calling entities multi ent
--Requires this to have a ComponentSpawner and ComponentMultiEnt
function attach_visual_sprite(aniSetName, zOffset, rotationSpeed, minAmbient, fadeIn) 
	local e = Entity.create(EntityGeneric)

	local _, _, offset = this:get_component(ComponentPosition):get_position() 
	e:add_component(ComponentPosition.create(0, 0, offset + zOffset))
	e:add_component(ComponentGraphics.create(aniSetName))
	e:add_component(ComponentRotation.create(0, rotationSpeed))

	if not is_nil(minAmbient) then e:add_component(ComponentMinAmbient.create(minAmbient)) end
	if not is_nil(fadeIn) then e:add_component(ComponentTransparency.create(1.0, 1.0/fadeIn)) end

	e:add_component(ComponentOffsetMaster.create(true))
	e:add_component(ComponentDieWithMaster.create())

	this:get_component(ComponentSpawner):add_entity(e)
end

--general function to load a level by setting the correct values in the Registar
--first parameter is state: title, level
--second parameter is campaign name, valid on title state
--third parameter is level number, valid on title state
function load_state(...)
	--Handling erreoneos event parameters will be handled by the engine's event handler
	emit_event(EventStateChange, ...)
end

--Test
function test() 
	load_state("level", "ascent", 1)
end

function get_dump_report() 
	local dumpReport = string.format("Event size: %d\n", get_event_length())
	dumpReport = dumpReport 
		.. string.format("Entities alive, Total: %04d Cached: %02d \n", EntityPool.get_entity_count())
	dumpReport = dumpReport 
		.. string.format("Graveyard, size: %03d passed: %05d \n", EntityPool.get_graveyard_size(), EntityPool.get_graveyard_passed())
	dumpReport = dumpReport 
		.. string.format("Memory Usage: Engine: %02f Mb, ScriptENV: %02f Mb", getVMemUsed() / 1000000, collectgarbage("count") / 1000)
	return dumpReport
end

function dump()
	print(get_dump_report())
end

function test2()
	local ent = EntityPool.get_cached_entity(EntityPlayer)
	local comp = ent:get_component(ComponentPosition)
	print(comp:get_position())
end

--Some layer constants

LAYER_PLAYER_BULLETS = 2
LAYER_PLAYER = 1
LAYER_ENEMY_BULLETS = 0
LAYER_ENEMY = -1

-- Add some debug variables
getGlobalRegistar():add("debug_render_hitbox", false)

Debug = {}

Debug.set_render_hitbox = function(value)
	GlobalRegistar.update("debug_render_hitbox", value)
end