---Extended library
--Classes and mg functionality have been bounded

--global pool, contains all the entities in the game
pool = getEntityPool()

--global registar, keeps states
registar = getGlobalRegistar()

--Add a function within Entity class to allow components to be retrived 
--by component name, returns nil if the entity does not have the specified component
function Entity.get_component(self, component)
	ptr = self:getComponent(component.type())
	if ptr == nil then
		return nil
	else
		return component.cast(ptr)
	end
end

--add a function to Registar to allow weak typed Registar return
--returns nil on failure
function Registar.get(self, name)
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
	emit_event(EventStateChange, ...)
end

--Test
function test() 
	load_state("level", "ascent", 1)



end