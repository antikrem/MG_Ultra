--\scripts\_initialise\death_scripts.lua

-- Map of entity type to a script
DeathScripts = {}

-- Add a callback for entity type
DeathScripts.add_callback = function(entityType, callback)
	DeathScripts[entityType] = callback
end

-- Executes callback against this
DeathScripts._execute_callback = function(entityType)
	if DeathScripts[entityType] ~= nil then
		DeathScripts[entityType](this)
	end
end