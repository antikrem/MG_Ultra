-- \scripts\_initialise\gameplay.lua

-- Manages some functions to modify gameplay
Gameplay = {}

-- Plays spaces in different gameplay modes
Gameplay.HEIGHT = 1080 / 2
Gameplay.GENERAL_WIDTH = 1440 / 2
Gameplay.EXTEND_WIDTH = 1920 / 2

-- Deletes all bullets
Gameplay.clear_bullets = function()
	EntityPool.kill_by_type(EntityEnemyBullet)
end

-- Deletes all enemies
Gameplay.clear_enemies = function()
	EntityPool.kill_by_type(EntityEnemy)
end

-- Extends gamplay arena
Gameplay.extend = function(frames)
	if is_nil(frames) then frames = 200 end
	local rate = tostring(1.0/frames)

	UI.hide_panes(frames)
	local p = EntityPool.get_player()

	if not is_nil(p) then
		p:get_component(ComponentClampPosition):set_dimensions(Gameplay.EXTEND_WIDTH, Gameplay.EXTEND_WIDTH)
	end
	
	local e = Entity.create(EntityUIElement)
	e:add_component(ComponentPosition.create(0,0,0))
	local t = ComponentText.create("text_consolas58")
	t:set_text("Extend")
	e:add_component(t)

	EntityPool.add_entity(e)
end