---sets g-registar variables, these are used for game control
registar = getGlobalRegistar();

----Engine level variables

--levels are on cycles, with scripted events occuring on the tick
registar:add("cycle", -1)
registar:add("cycle_progress", false)

--load request can be put anytime with appropiate events
--events will set these values, don't touch them otherwise
registar:add("load_request", false)
registar:add("loading", false)

--changing state/campaign/level is done with evets
--consider these read only methods
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
