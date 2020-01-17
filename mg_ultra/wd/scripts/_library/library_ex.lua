---Extended library
--Classes and mg functionality have been bounded

--global registar, keeps states
registar = getGlobalRegistar()

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
function Entity.add_component(self, component)
	self:addComponent(component.type(), component)
end

--add a function to Registar to allow weak typed Registar return
--returns nil on failure
function Registar.get(self, name)
	local v, check
	--implementation is done by attempting different acessors
	v, check = self:getInt(name)
	if check then
		return v
	end

	v, check = self:getFloat(name)
	if check then
		return v
	end

	v, check = self:getBool(name)
	if check then
		return v
	end

	v, check = self:getString(name)
	if check then
		return v
	end

	return nil
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

