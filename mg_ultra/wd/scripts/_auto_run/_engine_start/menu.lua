-- \scripts\_auto_run\_engine_start\menu.lua


Menu = {}

-- Creates a menu object
-- Which represents a single button

-- Menu for 
IngameMenu = {}

IngameMenu.active = false

IngameMenu.difficulty = Gameplay.EASY

IngameMenu.cursor = 0

IngameMenu.TokenEntities = { }

IngameMenu.cursorX = 1
IngameMenu.cursorY = 3

-- Look ups for the next row to go to (up, down)
IngameMenu.lookUp = {
	[3] = {5, 6},
	[5] = {6, 3},
	[6] = {3, 5}
}

-- Spawn a token at given (x, y) and return 
IngameMenu.spawn_token = function(text, x, y)
	local e = Entity.create(EntityUIElement)

	e:add_component(ComponentPosition.create(x, y, -10))
	local t = ComponentText.create("text_consolas58")
	t:set_text(text)
	t:set_render_in_3D(false)
	e:add_component(t)
	e:add_component(ComponentName.create(text))

	return e
end


IngameMenu.start = function()
	Gameplay.pause()

	local e = Entity.create(EntityFocusedMenu)
	e:add_component(ComponentMultiEntity.create())
	e:add_component(ComponentSpawner.create())
	e:add_component(ComponentInput.create())

	-- Add tokens
	for level in irange(1, #IngameMenu.Tokens ) do

		local tokens = IngameMenu.Tokens[level]
		local tokenEnts = {}

		for k,v in pairs(tokens) do
			local sub = IngameMenu.spawn_token(v[3], v[1], v[2])
			e:get_component(ComponentSpawner):add_entity(sub)

			table.insert(tokenEnts, sub)
		end

		IngameMenu.TokenEntities[level] = tokenEnts

	end

	-- Add cursor
	local target = IngameMenu.TokenEntities[IngameMenu.cursorY][IngameMenu.cursorX]
	local x, y = target:get_component(ComponentPosition):get_position()
	local cursor = IngameMenu.spawn_token(">", -20, y)
	e:get_component(ComponentSpawner):add_entity(cursor)

	EntityPool.add_cached_entity(e)

	IngameMenu.active = true

	-- Set some defaults
	IngameMenu.xOn()
	IngameMenu.cursorX = 1
	IngameMenu.cursorY = 5
	IngameMenu.xOn()
	IngameMenu.cursorX = 1
	IngameMenu.cursorY = 3
end

IngameMenu.moveCursor = function()
	local cursor = EntityPool.get_cached_entity(EntityFocusedMenu):get_component(ComponentMultiEntity):get(">")
	local target = IngameMenu.TokenEntities[IngameMenu.cursorY][IngameMenu.cursorX]
	local _, y = target:get_component(ComponentPosition):get_position()
	cursor:get_component(ComponentPosition):set_position(-20, y)
end

IngameMenu.xOff = function()
	local e = IngameMenu.TokenEntities[IngameMenu.cursorY][IngameMenu.cursorX]
	local text = IngameMenu.Tokens[IngameMenu.cursorY][IngameMenu.cursorX][3]
	e:get_component(ComponentText):set_text(text)
end

IngameMenu.xOn = function()
	local e = IngameMenu.TokenEntities[IngameMenu.cursorY][IngameMenu.cursorX]
	local text = IngameMenu.Tokens[IngameMenu.cursorY][IngameMenu.cursorX][3]
	if text:sub(1, 3) == "[ ]" then
		text = text:replace_char(2, "x")
	end
	e:get_component(ComponentText):set_text(text)
end

IngameMenu.xHas = function(x, y)
	local e = IngameMenu.TokenEntities[y][x]
	local text = e:get_component(ComponentText):get_text()
	return text:sub(1, 3) == "[x]"
end

IngameMenu.xWrap = function()
	IngameMenu.cursorX = (IngameMenu.cursorX % #IngameMenu.Tokens[IngameMenu.cursorY])
	if IngameMenu.cursorX == 0 then
		IngameMenu.cursorX = #IngameMenu.Tokens[IngameMenu.cursorY]
	end
end

IngameMenu.execute = function()
	local callback = IngameMenu.Tokens[IngameMenu.cursorY][IngameMenu.cursorX][4]

	if not is_nil(callback) then
		callback()
	end
end

IngameMenu.move = function(stepX, stepY) 
	local update = false

	if stepY < 0 then
		IngameMenu.cursorY = IngameMenu.lookUp[IngameMenu.cursorY][2]
		update = true
	elseif stepY > 0 then
		IngameMenu.cursorY = IngameMenu.lookUp[IngameMenu.cursorY][1]
		update = true
	elseif stepX < 0 then
		IngameMenu.xOff()
		IngameMenu.cursorX = IngameMenu.cursorX - 1
		IngameMenu.xWrap()
		IngameMenu.xOn()
		update = true
	elseif stepX > 0 then
		IngameMenu.xOff()
		IngameMenu.cursorX = IngameMenu.cursorX + 1
		IngameMenu.xWrap()
		IngameMenu.xOn()
		update = true
	end

	if update then
		IngameMenu.xWrap()
		IngameMenu.moveCursor()
	end
end


-- Starts game with these settings
IngameMenu.done = function()
	if IngameMenu.xHas(1, 3) then
		Gameplay.difficulty = Gameplay.EASY
	end

	if IngameMenu.xHas(2, 3) then
		Gameplay.difficulty = Gameplay.HARD
	end

	EntityPool.get_cached_entity(EntityFocusedMenu):get_component(ComponentMultiEntity):kill_children()
	EntityPool.get_cached_entity(EntityFocusedMenu):kill()

	IngameMenu.active = false

	Gameplay.unpause()
end

IngameMenu.update = function()
	if not IngameMenu.active then
		return
	end

	local e = EntityPool.get_cached_entity(EntityFocusedMenu)

	local input = e:get_component(ComponentInput)
	input:update()

	-- Check enter
	if input:query_press("console_enter") > 0 then
		IngameMenu.execute()
		return
	end

	local stepX = 0
	local stepY = 0

	-- Check movement input
	if input:query_press("up") > 0 then
		stepY = -1
	elseif input:query_press("down") > 0 then
		stepY = 1
	elseif input:query_press("left") > 0 then
		stepX = -1
	elseif input:query_press("right") > 0 then
		stepX = 1
	end
	
	IngameMenu.move(stepX, stepY)
	
end

IngameMenu.Tokens = {
	[1] = { {0, 60, "The King of Heaven Demo" } },
	[2] = { {0, 0, "Difficulty:"} },
	[3] = { {0, -60,"[ ] Easy"},  {200, -60, "[ ] Hard"} },
	[4] = { {0, -120, "Character:"} },
	[5] = { {0, -180, "[ ] Clergyman"} },
	[6] = { {0, -300, "[START]", IngameMenu.done} },
	[7] = { {390, -60,"[ ] HELL(todo)"}  }

}