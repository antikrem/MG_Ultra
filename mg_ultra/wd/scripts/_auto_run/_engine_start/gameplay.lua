-- \scripts\_initialise\gameplay.lua

-- Manages some functions to modify gameplay
Gameplay = {}

-- Deletes all bullets
Gameplay.clear_bullets = function()
	EntityPool.kill_by_type(EntityEnemyBullet)
end

-- Deletes all enemies
Gameplay.clear_enemies = function()
	EntityPool.kill_by_type(EntityEnemy)
end