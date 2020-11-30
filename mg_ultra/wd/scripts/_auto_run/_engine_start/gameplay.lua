-- \scripts\_initialise\gameplay.lua

-- Manages some functions to modify gameplay
Gameplay = {}

Gameplay.clear_bullets = function()
	EntityPool.kill_by_type(EntityEnemyBullet)
end

