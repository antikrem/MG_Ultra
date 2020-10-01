-- \scripts\_auto_run\_engine_start\weather.lua

WeatherTypes = {}
WeatherTypes.None = "None"
WeatherTypes.HeavyRain = "HeavyRain"

-- Current weather is saved as a string
registar:add("weather_type", WeatherTypes.None)

-- Particles used for weather
Particles.add_new_type("weather_heavyrain", "weather_rain0")
Particles.set_box_dimension("weather_heavyrain", 960, 540, 700)
Particles.set_box_center("weather_heavyrain", 0, 0, -160)
Particles.set_weight("weather_heavyrain", 0.00, 0.00)
Particles.set_type_response("weather_heavyrain", ParticleBoxResponse.Wrap)
Particles.set_featherness("weather_heavyrain", 0.05, 0.019)
Particles.set_scrollness("weather_heavyrain", 1)
Particles.set_rotate_to_face("weather_heavyrain", true)
Particles.set_velocity_max_rotation("weather_heavyrain", 10)

Particles.add_new_type("weather_darkcloud", "weather_cloud0", 1, 2.4)
Particles.set_box_dimension("weather_darkcloud", 1500, 2300, 790)
Particles.set_box_center("weather_darkcloud", 0, 0, 800)
Particles.set_featherness("weather_darkcloud", 0.00, 0.00)
Particles.set_scrollness("weather_darkcloud", 1)
Particles.set_type_response("weather_darkcloud", ParticleBoxResponse.Wrap)

Particles.add_new_type("weather_frontcloud", "weather_cloud0", 1, 2.4)
Particles.set_box_dimension("weather_frontcloud", 1920, 1080, 200)
Particles.set_box_center("weather_frontcloud", 0, 0, -200)
Particles.set_featherness("weather_frontcloud", 0.00, 0.00)
Particles.set_scrollness("weather_frontcloud", 1)
Particles.set_type_response("weather_frontcloud", ParticleBoxResponse.Wrap)

-- Assets for weather
Audio.request_load_file("rain", "rain.wav")
Audio.request_load_file("thunder_strike0", "strike0.wav")
Audio.request_load_file("thunder_rumble0", "thunder0.wav")
Audio.request_load_file("thunder_rumble1", "thunder1.wav")

Weather = {}

-- Pause toggle on music
Weather.toggle_audio_pause = function()
	local weatherEnt = EntityPool.get_cached_entity(EntityWeather)
	
	if not is_nil(weatherEnt) then
		local cAudio = weatherEnt:get_component(ComponentAudio)
		local state = cAudio:get_state()
		if state == AudioState.PLAY then
			cAudio:pause()
		elseif state == AudioState.PAUSE then
			cAudio:resume()
		end
	else
		print("WEATHER, Warning pausing weather ent audio, entity could not be found")
	end
end

-- Stops all weather
Weather.stop = function()
	-- Set weather flag 
	registar:update("weather_type", WeatherTypes.HeavyRain)

	Particles.clear_all()
	EntityPool.get_cached_entity(EntityWeather):get_component(ComponentAudio):stop()

	Drift.set_wind_speed(0, 0, 0)

end

-- Starts heavy rain
Weather.start_heavy_rain = function()
	-- Set weather flag 
	registar:update("weather_type", WeatherTypes.HeavyRain)

	-- Start rain
	EntityPool.get_cached_entity(EntityWeather):get_component(ComponentParticle):spawn_uniformly(900)
	EntityPool.get_cached_entity(EntityWeather):get_component(ComponentParticle):use_type("weather_darkcloud")
	EntityPool.get_cached_entity(EntityWeather):get_component(ComponentParticle):spawn_uniformly(9)
	EntityPool.get_cached_entity(EntityWeather):get_component(ComponentParticle):spawn_at(0, 0, 10)
	EntityPool.get_cached_entity(EntityWeather):get_component(ComponentParticle):use_type("weather_frontcloud")
	EntityPool.get_cached_entity(EntityWeather):get_component(ComponentParticle):spawn_uniformly(3)
	EntityPool.get_cached_entity(EntityWeather):get_component(ComponentAudio):play("rain")
	Drift.set_wind_speed(16, -20, 0)

	-- Moody lights and fog
	PostEffects.Exposure.set(1.1)
	AmbientLight.set_strength(0.02)
	Fog.set_colour(1.0, 1.0, 1.0)
	Fog.set_strength(0.0015)
	Fog.set_starting_depth(500)

	Weather.update_player_light()

	PostEffects.Bloom.set_threshold(0.2)
end

-- Can work if this is is player or player is in pool
Weather.update_player_light = function()
	local p = EntityPool.get_player() and EntityPool.get_player() or this
	if p and registar:get("weather_type") == WeatherTypes.HeavyRain then 
		p:get_component(ComponentPointLight):set_parameters(0.00068, 0, 1.5) 
	end
end

-- Strikes weak lightning
Weather.strike_weak_lightning = function(delay, fadeout, pX, pY, strength, r, g, b)
	if is_nil(delay) then delay = 10 end
	if is_nil(fadeout) then fadeout = 10 end

	if is_nil(pX) then pX = 1 end
	if is_nil(pY) then pY = 1 end

	if is_nil(strength) then strength = 0.1 end

	if is_nil(r) then pX = 1 end
	if is_nil(g) then pY = 1 end
	if is_nil(b) then pY = 1 end

	local e = Entity.create(EntityGeneric)
	local c = ComponentTimer.create()
	c:add_callback(
		1,
		"PostEffects.Bloom.set_threshold(1.0)" 
	)
	c:add_callback(
		delay,
		"this:get_component(ComponentDirectionalLight):set_strength(" 
			.. tostring(strength) ..", " 
			.. tostring(strength / fadeout) .. ", 0)" 
	)
	c:add_callback(
		delay + fadeout, 
		"this:kill() \n" ..
		"PostEffects.Bloom.set_threshold(0.2)" 
	)
	e:add_component(c)
	local d = ComponentDirectionalLight.create(pX, pY, -1.0, r, g, b)
	d:set_strength(strength)
	e:add_component(d)

	EntityPool.add_entity(e)
end

-- Strikes lightning
Weather.strike_lightning = function(bursts, pX, pY)
	local delay = 10
	local fadeout = 8

	if is_nil(bursts) then bursts = 3 end
	if is_nil(pX) then pX = 1 end
	if is_nil(pY) then pY = 1 end

	local e = Entity.create(EntityGeneric)
	local c = ComponentTimer.create()

	-- Add each burst
	c:add_callback(
		1,
		"PostEffects.Bloom.set_threshold(4.0)" 
	)
	c:add_callback(
		delay,
		"this:get_component(ComponentDirectionalLight):set_strength(4.0, " .. tostring(4.0 / fadeout) .. ", 0)" 
	)
	c:add_callback(
		delay + fadeout, 
		"this:kill() \n" ..
		"PostEffects.Bloom.set_threshold(0.2)" 
	)
	e:add_component(c)
	local d = ComponentDirectionalLight.create(pX, pY, -1.0, 2.0, 2.0, 2.0)
	e:add_component(d)

	EntityPool.add_entity(e)
end

-- Applies a blinding strike of lightning
Weather.strike_blinding_lightning = function(delay, fadeout)
	if is_nil(delay) then delay = 100 end
	if is_nil(fadeout) then fadeout = 100 end

	ColourModulation.set_cutoff(650)
	ColourModulation.set_background_colour(5, 5, 5)
	ColourModulation.set_foreground_colour(0,0,0)
	ColourModulation.set_strength(1.2)

	Fog.set_strength(0)

	local e = Entity.create(EntityGeneric)
	local c = ComponentTimer.create()
	c:add_callback(
		delay, 
		"ColourModulation.set_strength(1.2," .. tostring(1.2/fadeout) .. ", 0.000) " ..
		"Fog.set_strength(0, " .. tostring(0.0017/fadeout) .. ", 0.00165) " ..
		"this:get_component(ComponentDirectionalLight):set_strength(1.0, " .. tostring(1.0 / fadeout) .. ", 0)" 
	)
	c:add_callback(
		math.max(delay + fadeout, 1000), 
		"this:kill()"
	)
	e:add_component(c)
	local d = ComponentDirectionalLight.create(1.0, 0, -1.0, 1.0, 1.0, 1.0)
	e:add_component(d)
	e:add_component(ComponentAudio.create("thunder_strike0"))

	EntityPool.add_entity(e)
end

-- Strike lightning as a flash of powerful point lightning
Weather.strike_point_lightning = function(x, y, z, delay, fadeout, r, g, b, strength)
	
	local e = Entity.create(EntityGeneric)

	local c = ComponentTimer.create()

	-- Add each burst
	c:add_callback(
		delay,
		"this:get_component(ComponentPointLight):set_strength(0, " .. tostring(strength / fadeout) .. ")" 
	)
	c:add_callback(
		delay + fadeout, 
		"this:kill()"
	)
	e:add_component(c)
	
	e:add_component(ComponentPointLight.create(r, g, b, 0.00003, 0.001, 2))
	e:add_component(ComponentPosition.create(x, y, z))
	e:add_component(ComponentDriftable.create())
	e:add_component(ComponentNoBoundsControl.create())

	EntityPool.add_entity(e)
end

-- Strike lightning as a flash of powerful point lightning randomly
Weather.strike_random_point_lightning = function()
	local z = math.lerp(math.random(), 500, 700)

	local x,y = math.rand_vec(2, -1000, 1000)
	Weather.strike_point_lightning(
		x,y,z,
		10,
		2,
		10.0,10.0,10.0,
		1.0
	)
end

-- Strike lightning as a burst of multiple lights
Weather.strike_burst_lightning = function()
	
	local e = Entity.create(EntityGeneric)
	local c = ComponentTimer.create()
	c:add_callback(1, "Weather.strike_random_point_lightning()")
	c:add_callback(math.random(5, 9), "Weather.strike_random_point_lightning()")
	if math.random() > 0.5 then
		c:add_callback(math.random(12, 17), "Weather.strike_random_point_lightning()")
	end
	c:add_callback(100, "this:kill()")
	e:add_component(c) 
	e:add_component(ComponentAudio.create("thunder_rumble0"))

	EntityPool.add_entity(e)
end

-- Strike lightning as a burst of multiple lights, with heavy sound
Weather.strike_burst_lightning_alt = function()
	
	local e = Entity.create(EntityGeneric)
	local c = ComponentTimer.create()
	c:add_callback(1, "Weather.strike_random_point_lightning()")
	c:add_callback(math.random(5, 9), "Weather.strike_random_point_lightning()")
	if math.random() > 0.5 then
		c:add_callback(math.random(12, 17), "Weather.strike_random_point_lightning()")
	end
	c:add_callback(500, "this:kill()")
	e:add_component(c) 
	e:add_component(ComponentAudio.create("thunder_rumble1"))

	EntityPool.add_entity(e)
end

-- Strike lightning as burst with a bit of a blind
Weather.strike_blinding_burst_light = function()
	Weather.strike_burst_lightning_alt()
	Weather.strike_lightning()
end