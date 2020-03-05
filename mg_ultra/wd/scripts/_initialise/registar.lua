---sets g-registar variables, these are used for game control
registar = getGlobalRegistar();

--add a function to Registar to allow weak typed Registar return
--returns nil on failure
function Registar.get(self, name)
	local v, check
	--implementation is done by attempting different acessors
	v, check = self:getInt(name)
	if check then
		return v
	end

	v, check = self:getFloat(name)
	if check then
		return v
	end

	v, check = self:getBool(name)
	if check then
		return v
	end

	v, check = self:getString(name)
	if check then
		return v
	end

	return nil
end

--global registar modifiers
GlobalRegistar = {}

function GlobalRegistar.add(name, value)
	registar:add(name, value)
end

function GlobalRegistar.update(name, value)
	registar:update(name, value)
end

function GlobalRegistar.get(name)
	return registar:get(name)
end

----Engine level variables

--Pause Engine
registar:add("pause", false)

--levels are on cycles, with scripted events occuring on the tick
registar:add("cycle", -1)
registar:add("cycle_progress", false)
registar:add("cycle_in_dialogue", false)

--load request can be put anytime with appropiate events
--events will set these values, don't touch them otherwise
registar:add("load_request", false)
registar:add("loading", false)
--flag to start loading dialogue
registar:add("load_dialogue", false)
registar:add("in_dialogue", false)
registar:add("in_dialogue_pause", false)

--changing state/campaign/level is done with evets
--consider these read only values
registar:add("state", "none")
registar:add("campaign", "none")
registar:add("level", 0)
registar:add("next_state", "none")
registar:add("next_campaign", "none")
registar:add("next_level", 0)

---- Game specific parameters
--play space is horizontal/vertical area a player can move
--Each unit is a single pixel full screen at 1080p
--is also used fir cullling entities
registar:add("play_space_x", 1920)
registar:add("play_space_y", 1080)

--Used for culling entities that fall outside of the play space
registar:add("culling_enable", true)
registar:add("culling_padding", 100)

--Sets the vertical camera view, how  much of the play_space_y
--is viewable
registar:add("view_space_y", 1080)