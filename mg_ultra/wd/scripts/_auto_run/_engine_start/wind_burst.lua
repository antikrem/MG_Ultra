-- \scripts\_auto_run\_engine_start\weather.lua

-- can add a strong burst of wind to the scene
WindBurst = {}

WindBurst._burst_list = {}

-- Duration between bursts
WindBurst._cooldown = 1000

-- Count to next burst
WindBurst._last_cool_down = 0

-- true when in a burst
WindBurst._in_burst = false

-- Current burst when in burst
WindBurst._burst = {0, 0, 0, 0}

-- Add a wind burst
WindBurst.add_burst = function(x, y, z, length)
	arrays.append(WindBurst._burst_list, {x, y, z, length})
end

-- Delete all wind bursts
WindBurst.delete_bursts = function()
	WindBurst._burst_list = {}
end	

-- Get a random winburst
WindBurst.get_burst = function()
	local size = #WindBurst._burst_list
	if size == 0 then return nil end
	if size == 1 then return WindBurst._burst_list[1] end
	return WindBurst._burst_list[math.random(1, size)] 
end	

-- Update for WindBurst
WindBurst._update = function()
	if not WindBurst._in_burst then
		WindBurst._last_cool_down = WindBurst._last_cool_down + 1
		
		--check if its time for a new burst
		if WindBurst._last_cool_down > WindBurst._cooldown then
			WindBurst._last_cool_down = 0
			WindBurst._burst = WindBurst.get_burst()
			
			if not is_nil(WindBurst._burst) then
				WindBurst._in_burst = true

				GlobalRegistar.update("drift_windspeed_x", GlobalRegistar.get("drift_base_windspeed_x") + WindBurst._burst[1])
				GlobalRegistar.update("drift_windspeed_y", GlobalRegistar.get("drift_base_windspeed_y") + WindBurst._burst[2])
				GlobalRegistar.update("drift_windspeed_z", GlobalRegistar.get("drift_base_windspeed_z") + WindBurst._burst[3])
			end
		end
	
	else
		WindBurst._last_cool_down = WindBurst._last_cool_down + 1
		
		--check if its time for last burst to end
		if WindBurst._last_cool_down > WindBurst._burst[4] then
			WindBurst._last_cool_down = 0
			WindBurst._in_burst = false

			GlobalRegistar.update("drift_windspeed_x", GlobalRegistar.get("drift_base_windspeed_x"))
			GlobalRegistar.update("drift_windspeed_y", GlobalRegistar.get("drift_base_windspeed_y"))
			GlobalRegistar.update("drift_windspeed_z", GlobalRegistar.get("drift_base_windspeed_z"))
		end
	end

end
