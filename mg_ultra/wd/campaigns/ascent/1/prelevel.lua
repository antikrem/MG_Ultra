bullet_converging_twin = function(offset)
	local playerAngle = get_angle_to_player(this:get_component(ComponentPosition):get_position())

	local e = Entity.create(EntityEnemyBullet)
	e:add_component(ComponentPosition.create(0, 0, 0.1))
	e:add_component(ComponentOffsetOnce.create())
	e:add_component(ComponentGraphics.create("bul_big_yellow"))
	e:add_component(ComponentPointLight.create(0, 0, 0, 0.9647, 0.8039, 0.3751, 0.004, 0.3, 4.85))
	e:add_component(ComponentMinAmbient.create(1.1))

	local bulletMComponent = ComponentMovement.create()
	bulletMComponent:set_speed(8)
	bulletMComponent:set_angle(playerAngle + offset)
	e:add_component(bulletMComponent)

	this:get_component(ComponentSpawner):add_entity(e)

	local e = Entity.create(EntityEnemyBullet)
	e:add_component(ComponentPosition.create(0, 0, 0.1))
	e:add_component(ComponentOffsetOnce.create())
	e:add_component(ComponentCollision.create(100))
	e:add_component(ComponentGraphics.create("bul_big_yellow"))
	e:add_component(ComponentPointLight.create(0.9647, 0.8039, 0.3751, 0.004, 0.3, 4.85))
	e:add_component(ComponentMinAmbient.create(1.1))

	local bulletMComponent = ComponentMovement.create()
	bulletMComponent:set_speed(8)
	bulletMComponent:set_angle(playerAngle - offset)
	e:add_component(bulletMComponent)

	this:get_component(ComponentSpawner):add_entity(e)
end



BulletSpawner.new(
	"default",
	{
		initialiserScript = false,
		scriptLookUp = {
			[0] = function() 
				attach_visual_sprite("mc_blue_triangle_i", 0, -2.0, 1.0, 20)
				attach_visual_sprite("mc_blue_triangle_o", 0, 2.0, 1.0, 20)
			end,
			[1] = false,
			[30] = function() 
				bullet_converging_twin(60)
			end,
			[31] = false,
			[50] = function() 
				bullet_converging_twin(40)
			end,
			[51] = false,
			[70] = function() 
				bullet_converging_twin(20)
			end,
			[71] = false,
			[90] = function() 
				bullet_converging_twin(0)
			end,
			[91] = false,
		},
		components = {
			{ComponentPosition, 0, 0, 0},
			{ComponentOffsetMaster, true},
			{ComponentDieWithMaster},
			{ComponentSpawner},
			{ComponentMultiEntity}
		}
	}
)