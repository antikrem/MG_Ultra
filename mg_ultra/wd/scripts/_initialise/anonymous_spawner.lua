--anonymous_spawner.lua
--creates a cached entity that allows the spawning of entities

AnonymousSpawner = {}

AnonymousSpawner.getAnonymousSpawner = function()
	return EntityPool.get_cached_entity(EntityAnonymousSpawner):get_component(ComponentSpawner)
end

--Creates an entity, add components with AnonymousSpawner.add_component
AnonymousSpawner.create_entity = function(entityType)
	entityType = entityType or ETGeneric
	return AnonymousSpawner.getAnonymousSpawner():create_entity(entityType)
end

--Adds a component to an entity made with AnonymousSpawner.create_entity
AnonymousSpawner.add_component = function(component)
	AnonymousSpawner.getAnonymousSpawner():add_component(component)
end

--Pushes current factory entity in AnonymousSpawner into EntityPool
AnonymousSpawner.push_entity = function()
	return AnonymousSpawner.getAnonymousSpawner():push_entity()
end