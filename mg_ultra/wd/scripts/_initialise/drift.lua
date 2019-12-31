--Sets up some variables used for 
--controlling drift from wind/level scroll

--Scroll speed of level
registar:add("drift_scrollspeed_x", 0.0)
registar:add("drift_scrollspeed_y", 0.0)
registar:add("drift_scrollspeed_z", 0.0)

--Speed from wind
registar:add("drift_windspeed_x", 0.0)
registar:add("drift_windspeed_y", 0.0)
registar:add("drift_windspeed_z", 0.0)

Drift = {}	

function Drift.set_scroll_speed(x, y, z)
	x = x or 1.0e-10
	y = y or 1.0e-10
	z = z or 1.0e-10

	registar:update("drift_scrollspeed_x", x)
	registar:update("drift_scrollspeed_y", y)
	registar:update("drift_scrollspeed_z", z)
end

function Drift.set_wind_speed(x, y, z)
	x = x or 1.0e-10
	y = y or 1.0e-10
	z = z or 1.0e-10

	registar:update("drift_windspeed_x", x)
	registar:update("drift_windspeed_y", y)
	registar:update("drift_windspeed_z", z)
end