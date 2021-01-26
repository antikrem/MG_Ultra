-- \scripts\_auto_run\_engine_start\boss.lua

-- Manages Boss fights
-- The Boss entity is a cached entity
-- That keeps a bullet master 
Boss = {}

-- Bullet spawners need to contain a component_name
-- With name "main"

-- The boss will always track its bullet spawner sub
-- on cycle 

-- Boss can be in 2 states
Boss.RESETTING = 0
Boss.PATTERN = 1
Boss.DONE = 2
Boss.state = Boss.RESETTING

Boss.RESET_LOCATION = {
		X = 0,
		Y = 300
	}


Boss.RESET_SPEED = 13

Boss.health = 0

-- A table that keeps a record of which bullet spawners to
-- cycle between (string name, number health)
Boss.spawnerTable = {
	{master = "boss_simple_spew", health = 100},
	{master = "boss_draw_in", health = 100},
	{master = "boss_simple_spew", health = 100}
}

-- Round within the spawner table
-- As soon as it overflows, resume cycle and play end script
Boss.round = 0



-- Spawn boss entity
Boss.spawn = function(animationset)
	local e = Entity.create(EntityBoss)
	e:add_component(ComponentPosition.create(0,0,0))
	e:add_component(ComponentGraphics.create("player", 0.3))

	e:add_component(ComponentCollision.create(50))

	e:add_component(ComponentMultiEntity.create())
	e:add_component(ComponentSpawner.create())

	-- Spawn an empty Bullet Master
	e:add_component(ComponentBulletMaster.create())

	EntityPool.add_cached_entity(e)

	Debug.track_variable("Boss.state", 5)
	Debug.track_variable("Boss.health", 5)

end

Boss.debug = function()
	local e = EntityPool.get_cached_entity(EntityBoss)
	print(e:report_components())
end

-- Kills existing pattern
Boss.kill_spawner_main = function()
	local e = EntityPool.get_cached_entity(EntityBoss)
	if not is_nil(e:get_component(ComponentMultiEntity):get("main")) then
		e:get_component(ComponentMultiEntity):get("main"):kill()
	end
end

-- Progress to go from RESET to PATTERN
-- Managing the patterns
Boss.progress_patttern = function(boss)
	-- If there exists an existing bullet spawner, delete
	Boss.kill_spawner_main()

	-- Set this bullet master to run new spawner
	boss:get_component(ComponentBulletMaster):reset_to(Boss.spawnerTable[Boss.round].master)

end

-- Progress boss to next stage
Boss.progress = function(boss)
	-- Standard progressions

	if Boss.state == Boss.RESETTING then
		Boss.round = Boss.round + 1
		Boss.state = Boss.PATTERN 
		
		if Boss.round > #Boss.spawnerTable then
			Boss.kill_spawner_main()
			Boss.state = Boss.DONE
		else
			Boss.health = Boss.spawnerTable[Boss.round].health
			Boss.progress_patttern(boss)
		end
	elseif Boss.state == Boss.PATTERN then
		Boss.state = Boss.RESETTING 

	end

end

-- Update to reset this boss 
Boss.reset_update = function(boss)
	local distance = math.distance_to(Boss.RESET_LOCATION.X, Boss.RESET_LOCATION.Y, boss:get_component(ComponentPosition):get_position())
	
	if distance < Boss.RESET_SPEED then
		boss:get_component(ComponentPosition):set_position(Boss.RESET_LOCATION.X, Boss.RESET_LOCATION.Y)
		Boss.progress(boss)
	else
		local x, y = boss:get_component(ComponentPosition):get_position()
		local angle = math.angle_to(x, y, Boss.RESET_LOCATION.X, Boss.RESET_LOCATION.Y)
		local dx, dy = math.to_point(Boss.RESET_SPEED, angle)
		boss:get_component(ComponentPosition):set_position(x + dx, y + dy)
	end

end

-- Update while pattern is running
Boss.pattern_update = function(boss)
	
	-- Keep boss at bullet spawner sub
	local e = boss:get_component(ComponentMultiEntity):get("main")
	if not is_nil(e) and not is_nil(e:get_component(ComponentPosition)) then
		boss:get_component(ComponentPosition):set_position(e:get_component(ComponentPosition):get_position())

	end
	
	-- Progress boss on low health
	if Boss.health <= 0 then
		Boss.progress()
	end
end

-- Boss update fucntion
Boss.update = function()
	-- Get active spawner
	local boss = EntityPool.get_cached_entity(EntityBoss)
	if is_nil(boss) then
		return
	end

	if Boss.state == Boss.RESETTING then
		Boss.reset_update(boss)
	elseif Boss.state == Boss.PATTERN then
		Boss.pattern_update(boss)
	end
	
end


-- Handler for boss receiving damage
Boss.damage_handle = function()
	local damage = target1:get_component(ComponentDamage):get_damage()
	target1:get_component(ComponentDamage):set_damage(0)
	Boss.health = Boss.health - damage
	target1:kill()

end