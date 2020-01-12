--Sets up the background master
--This allows background layer objects to be created

--Static accessor for controlling background
BackgroundMaster = {}

function BackgroundMaster.getBM() 
	return EntityPool.get_cached_entity(EntityBackgroundMaster);
end

--Functions for adding new layers to background master
--will use the idle aniamtion of given animation Sets
--depth is how deep in the level this entity exists in
--horizontalCount is the number of tiles that will apear to 
--stretch across the gamescreen
function BackgroundMaster.add_new_layer(animationSet, horizontalCount, depth)
	--check for animation set
	local aWidth, aHeight = AnimationMaster.query_animation_size(animationSet)

	if is_nil(aWidth) then
		print("BACKGROUNDMASTER, Error, amimationSet given to BackgroundMaster.add_new_layer is invalid")
		return
	end

	--get camera
	local camera = EntityPool.get_cached_entity(EntityCamera)
	if is_nil(camera) then
		print("BACKGROUNDMASTER, Error, No camera in existence")
		return
	end

	--Use the depth to create a translation factor
	local _, _, camDepth = camera:get_component(ComponentPosition):get_position()
	local translationFactor = (-1 * camDepth + depth) / (-1 * camDepth)

	--get the width and height of the plane
	local pWidth, pHeight = registar:get("play_space_x"), registar:get("play_space_y")

	--calculate the size of the tiles:
	local tWidth = (translationFactor * pWidth) / horizontalCount
	local tHeight = (tWidth / aWidth) * aHeight

	--calculate the number of tiles
	local tWidthCount = horizontalCount + 2
	local tHeightCount = math.ceil( (translationFactor * pHeight) / tHeight ) + 2
	local tileCount = math.max(tWidthCount, tHeightCount)

	--Background size
	local bWidth = tileCount * tWidth
	local bHeight = tileCount * tHeight

	print(bWidth/aWidth, bHeight/aHeight, tileCount)

	local bm = BackgroundMaster.getBM()
	local cSpawner = bm:get_component(ComponentSpawner)

	cSpawner:create_entity(EntityBackgroundLayer)

	cSpawner:add_component(ComponentPosition.create(0, 0, depth))

	local backgroundLayerGraphics = ComponentGraphics.create("default")
	backgroundLayerGraphics:set_animation(1)
	backgroundLayerGraphics:set_scale(bWidth/aWidth)
	backgroundLayerGraphics:set_wrap_factor(tileCount)
	cSpawner:add_component(backgroundLayerGraphics)

	cSpawner:add_component(ComponentDriftable.create())

	cSpawner:add_component(ComponentNoBoundsControl.create())

	--Add a scripting component to save 
	local cScript = ComponentExtendedScripting.create()
	cScript:add("tilesize_x", tWidth)
	cScript:add("tilesize_y", tHeight)
	cSpawner:add_component(cScript)

	cSpawner:push_entity()

end

--Updates a background layer
function update_background_layer(backgroundLayer)
	local cPos = backgroundLayer:get_component(ComponentPosition)
	local cEScript = backgroundLayer:get_component(ComponentExtendedScripting)
	
	local x, y = cPos:get_position()
	
	cPos:set_position(x % cEScript:get("tilesize_x"), y % cEScript:get("tilesize_y"))
end