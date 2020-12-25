---Extended library
-- Classes and mg functionality have been bounded

-- Add a function within Entity class to allow components to be retrived 
-- by component name, returns nil if the entity does not have the specified component
function Entity.get_component(self, component)
	if component == nil then
		print("ENTITY: Error, attempting to get a nil component from an entity")
		return nil;
	end

	local ptr = self:getComponent(component.type())
	if ptr == nil then
		return nil
	else
		return component.cast(ptr)
	end
end

-- Adding a component to an entity after its been put in a pool
-- will not attach them for modification from associated systems
-- will return false if operation failed
function Entity.add_component(self, component)
	return self:addComponent(component.type(), component)
end

-- Executes a script that uses "this" as target against an entity
function Entity.execute_against(self, f, ...)
	this = self
	f(...)
	this = nil
end

-- Pause and unpause the game
function pause()
	Weather.toggle_audio_pause()
	Music.toggle_pause()
	emit_event(EventPause)
end

-- Function to get angle from a position to player
-- Otherwise just angles down
function get_angle_to_player(x, y)
	
	local p = EntityPool.get_cached_entity(EntityPlayer)
	if is_nil(p) then return -90 end

	local p_x, p_y, angle
	p_x, p_y = p:get_component(ComponentPosition):get_position()

	_, angle = math.to_polar(p_x - x, p_y - y)
	return angle
end

-- Solves for the angle required by a bullet starting at x, y at speed s
-- To hit the player
-- Returns -450 if player isn't alive
function solve_angle_to_moving_player(bx, by, s)
	local player = EntityPool.get_player()

	if is_nil(player) then return -450 end

	local px, py = player:get_component(ComponentPosition):get_position()
	local cMovement = player:get_component(ComponentMovement)
	local vx, vy = math.to_point(cMovement:get_speed(), cMovement:get_angle())

	local j = px - bx;
	local k = py - by;

	-- Solution is in the form of a quadratic
	local a = (vy^2 + vx^2 - s^2);
	local b = (2*j*vx + 2*k*vy);
	local c = (j^2 + k^2);

	local t0, t1 = math.solve_quadratic(a, b, c)

	-- Select smallest non negative solution
	local solutions = {math.strip_negatives(t0, t1)}
	local t_star = nil
	if #solutions > 0 then
		t_star = math.min(unpack(solutions))
	end

	local theta = 0

	-- If the player is moving away faster than s, there will be no real non-negative solution
	-- In such a case, fallback to non-predictive target
	if is_nil(t_star) then
		theta = get_angle_to_player(bx, by)
	else
		theta = math.to_degrees(math.atan2((k + t_star * vy), (j + t_star * vx)))
	end

	return theta

end

-- Creates a sprite and attaches it to calling entities multi ent
-- Requires this to have a ComponentSpawner and ComponentMultiEnt
function attach_visual_sprite(aniSetName, zOffset, rotationSpeed, minAmbient, fadeIn, rotation, offsetX, offsetY, scale) 
	local e = Entity.create(EntityGeneric)

	if is_nil(rotation) then rotation = 0 end
	if is_nil(offsetX) then offsetX = 0 end
	if is_nil(offsetY) then offsetY = 0 end
	if is_nil(scale) then scale = 1.0 end

	local _, _, offset = this:get_component(ComponentPosition):get_position() 
	e:add_component(ComponentPosition.create(offsetX, offsetY, offset + zOffset))
	e:add_component(ComponentGraphics.create(aniSetName, scale))
	e:add_component(ComponentRotation.create(rotation, rotationSpeed))

	if not is_nil(minAmbient) then e:add_component(ComponentMinAmbient.create(minAmbient)) end
	if not is_nil(fadeIn) then e:add_component(ComponentTransparency.create(1.0, 1.0/fadeIn)) end

	e:add_component(ComponentOffsetMaster.create(true))
	e:add_component(ComponentDieWithMaster.create())

	this:get_component(ComponentSpawner):add_entity(e)
end


function get_dump_report() 
	local dumpReport = string.format("Event size: %d\n", get_event_length())
	dumpReport = dumpReport 
		.. string.format("Entities alive, Total: %04d Cached: %02d \n", EntityPool.get_entity_count())
	dumpReport = dumpReport 
		.. string.format("Graveyard, size: %03d passed: %05d \n", EntityPool.get_graveyard_size(), EntityPool.get_graveyard_passed())
	dumpReport = dumpReport 
		.. string.format("Memory Usage: Engine: %02f Mb, ScriptENV: %02f Mb", getVMemUsed() / 1000000, collectgarbage("count") / 1000)
	return dumpReport
end

function dump()
	print(get_dump_report())
end

function test2()
	local ent = EntityPool.get_cached_entity(EntityPlayer)
	local comp = ent:get_component(ComponentPosition)
	print(comp:get_position())
end

-- Some layer constants

LAYER_PLAYER_BULLETS = 6
LAYER_PLAYER = 4
LAYER_ENEMY_BULLETS = 2
LAYER_ENEMY = 0

-- Add some debug variables
getGlobalRegistar():add("debug_render_hitbox", false)

Debug = {}

g_isRenderingHitbox = false;

Debug.set_render_hitbox = function(value)
	g_isRenderingHitbox = value
	GlobalRegistar.update("debug_render_hitbox", value)
end

Debug.render_hitbox = function() 
	g_isRenderingHitbox = not g_isRenderingHitbox
	GlobalRegistar.update("debug_render_hitbox", g_isRenderingHitbox)
end

Debug.show_cycle = function()
	local e = Entity.create(EntityDebugCycle)
	e:add_component(ComponentPosition.create(850, -500))
	local c = ComponentText.create("text_consolas58")
	c:set_render_in_3D(false)
	c:set_text("Cycle: ")
	
	e:add_component(c)
	c = ComponentTimer.create()
	c:add_spam_callback("this:get_component(ComponentText):set_text(GlobalRegistar.get(\"cycle\"))")
	e:add_component(c)
	EntityPool.add_cached_entity(e)
end

Debug.hide_cycle = function()
	local e = EntityPool.get_cached_entity(EntityDebugCycle)
	if e then e:kill() end
end

Debug.run_demo = function()
	load_state("level", "demo", 1)
end

run_demo = Debug.run_demo

Debug.run_test = function()
	load_state("level", "test", 1)
end

Debug.run_example = function()
	load_state("level", "example", 1)
end

Debug.scroll_to = function(cycle)
	GlobalRegistar.update("cycle", cycle)
	GlobalRegistar.update("cycle_spawn_ents", false)
end

run_test = Debug.run_test

-- General function to load a level by setting the correct values in the Registar
-- First parameter is state: title, level
-- Second parameter is campaign name, valid on title state
-- Third parameter is level number, valid on title state
function load_state(...)
	--Handling erreoneos event parameters will be handled by the engine's event handler
	emit_event(EventStateChange, ...)
end
 

Debug.tracker_offset = 0

Debug.track_variable = function(name)
	local e = Entity.create(EntityVariableTracker)
	e:add_component(ComponentPosition.create(-900, 56 * Debug.tracker_offset, -5))
	local c = ComponentText.create("text_consolas58")
	c:set_render_in_3D(false)
	c:set_text("TRACKING")
	c:set_scale(0.5)
	e:add_component(c)

	local tim = ComponentTimer.create()
	tim:add_spam_callback(
		"local value = vtostring(" .. name .. ") this:get_component(ComponentText):set_text(\"" .. name .. ": \" .. value)" 
		
	)
	e:add_component(tim)

	EntityPool.add_entity(e)

	Debug.tracker_offset =  Debug.tracker_offset + 1
 end