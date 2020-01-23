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
	initialise = function(self, entity)
		for _, comps in ipairs(self.components) do
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
			BulletSpawnerList[name]:initialise(blankEntity)
			registar:update("bullet_spawner_initialisation_success", 1)
		else 
			registar:update("bullet_spawner_initialisation_success", 0)
		end
	end

}

--Flag for last Bullet Spawner initialisation sucess
registar:add("bullet_spawner_initialisation_success", 1)