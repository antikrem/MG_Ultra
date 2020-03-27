-- Adds utilities to interact with the pool

-- Global pool, contains all the entities in the game
pool = getEntityPool()

---Enumeration of different types of entities
EntityNoType = 0
EntityGeneric = 1
EntityGameControl = 2
EntityConsole = 3
EntityCamera = 4
EntityPlayer = 5
EntityPlayerBullets = 6
EntityBackgroundMaster = 8
EntityBackgroundLayer = 9
EntityAnonymousSpawner = 10
EntityDirectionalLight = 11
EntityEnemy = 12
EntityMusic = 13
EntityWeather = 14
EntityBulletSpawner = 15
EntityEnemyBullet = 16
EntityDialogue = 17
EntityDebugCycle = 18
EntityScoreBoard = 19
EntityPowerBoard = 20
EntityFragmentBoard = 21
EntityPowerUp = 22

-- Adds to pool, returns boolean success
function EntityPool.add_entity(entity)
	return pool:add(entity)
end

-- Adds to pool and cache, returns boolean success
function EntityPool.add_cached_entity(entity)
	return pool:addCached(entity)
end

-- Returns the entity cached with this types
-- or nil on failure due to invalid type or no respective cached ent
function EntityPool.get_cached_entity(entityType)
	return pool:getEntFromCache(entityType)
end

function EntityPool.get_entity_count()
	return pool:size()
end

-- Gets the number of entities in the graveyard
function EntityPool.get_graveyard_size() 
	return pool:getGraveyardSize()
end

-- Gets the number of entities passed by the pool
function EntityPool.get_graveyard_passed() 
	return pool:getGraveyardPassed()
end

-- Attempts to kill an cached entity
-- Returns boolean success
function EntityPool.kill_cached_entity(entityType)
	local ent = EntityPool.get_cached_entity(entityType)
	if ent then ent:kill() return true else return false end
end

-- Attempts to get player, might get nil
function EntityPool.get_player()
	return EntityPool.get_cached_entity(EntityPlayer)
end