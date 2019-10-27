--Adds utilities to interact with the pool

--global pool, contains all the entities in the game
pool = getEntityPool()

---Enumeration of different types of entities
--
EntityNoType = 0
EntityGeneric = 1
EntityConsole = 2
EntityCamera = 3
EntityPlayer = 4

--returns the entity cached with this types
--or nil on failure due to invalid type or no respective cached ent
function EntityPool.get_cached_entity(entityType)
	return pool:getEntFromCache(entityType)
end

function EntityPool.get_entity_count()
	return pool:size()
end

--gets the number of entities in the graveyard
function EntityPool.get_graveyard_size() 
	return pool:getGraveyardSize()
end