-- \scripts\_initialise\collision.lua

Collision = {}


--Add new collision event
Collision.add_event = function(source, target, script)
	if is_nil(script) then
		setCollisionEvent(source, target)
	else
		setCollisionEventWithScript(source, target, script)
	end
end