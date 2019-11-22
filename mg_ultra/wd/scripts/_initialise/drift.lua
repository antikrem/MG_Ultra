--Sets up some variables used for 
--controlling drift from wind/level scroll
registar:add("drift_scrollspeed_x", 0.0)
registar:add("drift_scrollspeed_y", 0.0)
registar:add("drift_scrollspeed_z", 0.0)

Drift = {}	

function Drift.set_scroll_speed(x, y, z)
	x = x or 1.0e-10
	y = y or 1.0e-10
	z = z or 1.0e-10

	registar:update("drift_scrollspeed_x", x + 1.0e-10)
	registar:update("drift_scrollspeed_y", y + 1.0e-10)
	registar:update("drift_scrollspeed_z", z + 1.0e-10)
end