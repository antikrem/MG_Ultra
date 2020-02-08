-- scripts\player.lua
-- executed regularly to update player
-- "this" refers to player


-- Standard player handle
if GlobalRegistar.get("player_alive") then
	g_sequentialDeadFrames = 0
	g_playerMovementUpdate()
	g_playerSpawnBullets()

-- Dead player handle
else
	g_sequentialDeadFrames = g_sequentialDeadFrames + 1

	local y 
		= (g_sequentialDeadFrames / PLAYER_DEAD_SCROLL) * PLAYER_DEAD_SCROLL_END_Y
		+ (1 - g_sequentialDeadFrames / PLAYER_DEAD_SCROLL) * PLAYER_DEAD_SCROLL_START_Y

	this:get_component(ComponentPosition):set_position(0, y, -5)

	if (PLAYER_DEAD_SCROLL < g_sequentialDeadFrames) then
		this:get_component(ComponentClampPosition):set_active(true)
		GlobalRegistar.update("player_alive", true)
	end
end