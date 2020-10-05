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
EntityVariableTracker = 23
EntityAnonymousSoundSource = 24

-- Adds to pool, returns boolean success
function EntityPool.add_entity(entity)
	return pool:addSingular(entity)
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

-- Kills all entities in the pool
function EntityPool.kill_all()
	 return pool:killByType(EntityNoType)
end

-- Kills all entities by type
function EntityPool.kill_by_type(type)
	 return pool:killByType(type)
end
-- Attempts to get player, might get nil
function EntityPool.get_player()
	return EntityPool.get_cached_entity(EntityPlayer)
end

-- Attempts to get camera, might get nil
function EntityPool.get_camera()
	return EntityPool.get_cached_entity(EntityCamera)
end