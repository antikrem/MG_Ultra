-- scripts\player.lua
-- executed regularly to update player when player is enabled
-- "this" refers to player


-- Standard player handle
if GlobalRegistar.get("player_alive") then
	Player.sequentialDeadFrames = 0
	Player.preDeadFrames = 0
	g_playerMovementUpdate()
	g_playerSpawnBullets()
	g_playerPowerUpdate()
	Player.friend_handle()
	Player.player_magnet_check()

-- Dead player handle
else
	local y = 0

	if Player.preDeadFrames < Player.PRE_DEAD_SCROLL then 
		Player.preDeadFrames = Player.preDeadFrames + 1
		y = 1000
	else 
		if Player.sequentialDeadFrames == 5 and Player.clearOnDeath then Player.add_clear_aura() Player.clearOnDeath  = false end

		Player.sequentialDeadFrames = Player.sequentialDeadFrames + 1

		y = (Player.sequentialDeadFrames / PLAYER_DEAD_SCROLL) * PLAYER_DEAD_SCROLL_END_Y
		  + (1 - Player.sequentialDeadFrames / PLAYER_DEAD_SCROLL) * PLAYER_DEAD_SCROLL_START_Y
	end

	this:get_component(ComponentPosition):set_position(0, y, LAYER_PLAYER_BULLETS)

	if (PLAYER_DEAD_SCROLL < Player.sequentialDeadFrames) then
		this:get_component(ComponentClampPosition):set_active(true)
		GlobalRegistar.update("player_alive", true)
	end
end