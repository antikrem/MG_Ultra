--\scripts\bullet_system\bullet_spawners.lua

--The spawning of bullets is handled by bullet spawners
--These entities will be attached as sub entities to enemies

--Each bullet spawner is based on a spawner template
--And a bullet spawner will look up its coresponding bullet template
--and execute any scripts

--This is the class used for generating bullet templates
BulletSpawner = {
	--Initialiser function ran before any other script
	--or attachement to an entity
	initialiserScript = false,

	--look up table for scripts
	--a looked up script will be repeated until
	--another script or false entry is founf
	scriptLookUp = {},

	--Used for adding components to the bullet spawner entity
	--before the initialiser script is run
	components = {},

	 --use to spawn new bullet spawner templates
	new = function(name, obj)
		obj = obj or {} 
		setmetatable(obj, BulletSpawner)
		BulletSpawner.__index = BulletSpawner

		--Add to global spawner list
		BulletSpawnerList[name] = obj
		
		return obj
    end,

	--Allows a bullet spawner entity to be initialised from 
	--this spawner template
	initialise = function(self, entity, name)
		entity:add_component(ComponentExtendedScripting.create())
		for _, comps in ipairs(self.components) do
			if is_nil(comps[1]) 
				then print("BULLETSPAWNER, Error, nil component loading: " .. name .. " indexed at " .. _) return 
			end

			entity:add_component(comps[1].create(unpack(comps, 2)))
		end
	end
}

--A table of all BulletSpawners made
--Which is acessed when looking up for bullet spawners
BulletSpawnerList = {
	--Takes a blank entity and initialises its
	--will set the success value in Flag
	_initialise = function(blankEntity, name)
		if BulletSpawnerList[name] ~= nil then
			BulletSpawnerList[name]:initialise(blankEntity,name)
			registar:update("bullet_spawner_initialisation_success", 1)
		else 
			registar:update("bullet_spawner_initialisation_success", 0)
		end
	end,

	--Takes bullet spawner name, current cycle and last valid cycle
	--will execute correct 
	_spawnerUpdate = function(name, current, lastValid)
		--if found then update last valid
		if BulletSpawnerList[name].scriptLookUp[current] ~= nil then
			--need to check if this look up is a function or false (end of last active)
			if BulletSpawnerList[name].scriptLookUp[current] == false then
				lastValid = -1
				this:get_component(ComponentBulletSpawner):setLastActive(-1)
			else
				lastValid = current
				this:get_component(ComponentBulletSpawner):setLastActive(current)
			end
			
		end
		
		--If theres a postive last active, run script
		if lastValid >= 0 and this:is_alive() then
			BulletSpawnerList[name].scriptLookUp[lastValid]()
		end
	end

}

--Flag for last Bullet Spawner initialisation sucess
registar:add("bullet_spawner_initialisation_success", 1)