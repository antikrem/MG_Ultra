-- \scripts\_initialise\power_ups.lua

POWER_UP_SPAWN_RADIUS = 80
POWER_UP_SPEED_CAP = 8
POWER_UP_ACCELERATION = 0.12

POWER_UP_SCALE = 0.8

POWER_UP_MAGNET_RANGE = 150
POWER_UP_COLLISION_RANGE = 40

POWER_UP_ACCELERATION_TO_PLAYER = 0.5

POWER_UP_POWER_POINTS = 1

POWER_UP_TYPES = {}
POWER_UP_TYPES.POWER = "pu_power"
POWER_UP_TYPES.FRAGMENT = "pu_fragment"

-- Spawn a power up
g_spawn_individual_powerup = function(powerupType)
	local x, y = this:get_component(ComponentPosition):get_position()
	local off_x, off_y = math.sample_uniform_circle(POWER_UP_SPAWN_RADIUS)

	local e = Entity.create(EntityPowerUp)
	e:add_component(ComponentGraphics.create(powerupType, POWER_UP_SCALE))
	e:add_component(ComponentPosition.create(x + off_x, y + off_y))
	e:add_component(ComponentMinAmbient.create(0.6))
	e:add_component(ComponentPointLight.create(1.0, 0.75, 0.05, 0.0005, 0.001, 4))
	e:add_component(ComponentCollision.create(POWER_UP_MAGNET_RANGE))
	e:add_component(ComponentMagnetiseToPlayer.create(POWER_UP_ACCELERATION_TO_PLAYER))
	local compMove = ComponentMovement.create()
	compMove:set_speed_cap(POWER_UP_SPEED_CAP)
	compMove:set_speed_change(POWER_UP_ACCELERATION)
	compMove:set_angle(-90)
	e:add_component(compMove)

	EntityPool.add_entity(e)
end

g_spawn_powerup = function()
	if this:get_component(ComponentBulletMaster) then
		local powerupName = this:get_component(ComponentBulletMaster):get_name()
		local powerupTable = BulletSpawnerList.getPowerUps(powerupName)

		for k, v in pairs(powerupTable) do
			replicate(v, g_spawn_individual_powerup, k)
		end

	else
		g_spawn_individual_powerup(POWER_UP_TYPES.POWER)
		g_spawn_individual_powerup(POWER_UP_TYPES.POWER)
		if math.random() < 0.3 then
			g_spawn_individual_powerup(POWER_UP_TYPES.POWER)
		end
	end
	
end

DeathScripts.add_callback(12, g_spawn_powerup)

-- Handles power up and player Collision
g_powerup_collision_handle = function()
	local x1, y1 = this:get_component(ComponentPosition):get_position()
	local x2, y2 = target1:get_component(ComponentPosition):get_position()

	local distance = math.to_magnitude(x1 - x2, y1 - y2);

	this:get_component(ComponentCollision):set_circle_radius(POWER_UP_COLLISION_RANGE)
	this:get_component(ComponentMagnetiseToPlayer):magnetise()

	if distance < POWER_UP_COLLISION_RANGE then
		-- Kill powerup
		this:kill()

		-- Based on what power up, modify state
		local powerupType = this:get_component(ComponentGraphics):get_animation_set()

		if powerupType == POWER_UP_TYPES.POWER then
			g_power = g_power + POWER_UP_POWER_POINTS
		elseif powerupType == POWER_UP_TYPES.FRAGMENT then 
			g_fragments = g_fragments + 1
			if g_fragments == g_nextFragments then
				g_lives = glives + 1
				g_nextFragments = g_nextFragments + FRAGMENT_PERIOD
			end
		end

	end
end

--Handler for power up being close to player
Collision.add_event(EntityPowerUp, EntityPlayer, "g_powerup_collision_handle()")