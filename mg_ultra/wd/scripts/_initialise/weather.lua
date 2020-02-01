--\scripts\_initialise\weather.lua

WeatherTypes = {}
WeatherTypes.None = "None"
WeatherTypes.HeavyRain = "HeavyRain"

--Current weather is saved as a string
registar:add("weather_type", WeatherTypes.None)

--particles used for weather
Particles.add_new_type("weather_heavyrain", "weather_rain0")
Particles.set_box_dimension("weather_heavyrain", 960, 540, 850)
Particles.set_box_center("weather_heavyrain", 0, 0, -400)
Particles.set_weight("weather_heavyrain", 0.00, 0.00)
Particles.set_type_response("weather_heavyrain", ParticleBoxResponse.Wrap)
Particles.set_rotate_to_face("weather_heavyrain", true)

--Assets for weather
Audio.request_load_file("rain", "rain.wav")

Weather = {}

--starts heavy rain
Weather.start_heavy_rain = function()
	--Set weather flag 
	registar:update("weather_type", WeatherTypes.HeavyRain)

	--Start rain
	EntityPool.get_cached_entity(EntityWeather):get_component(ComponentParticle):spawn_uniformly(900)
	EntityPool.get_cached_entity(EntityWeather):get_component(ComponentAudio):play("rain")
	Drift.set_wind_speed(10, -24, 0)

	--Moody lights and fog
	PostEffects.Exposure.set(1.1)
	AmbientLight.set_strength(0.01)
	Fog.set_colour(0.4, 0.4, 0.4)
	Fog.set_strength(0.00165)
	Fog.set_starting_depth(400)

	local p = EntityPool.get_player()
	if p then p:get_component(ComponentPointLight):set_parameters(0.00078, 0, 2) end

	PostEffects.Bloom.set_threshold(0.2)
end

--Applies a blinding strike of lightning
Weather.strike_blinding_lightning = function(delay, fadeout)
	if is_nil(delay) then delay = 100 end
	if is_nil(fadeout) then fadeout = 100 end

	ColourModulation.set_cutoff(1070)
	ColourModulation.set_background_colour(100000, 100000, 100000)
	ColourModulation.set_foreground_colour(0,0,0)
	ColourModulation.set_strength(1.2)

	Fog.set_strength(0)
	--AmbientLight.set_strength(1.2)

	local e = Entity.create(EntityGeneric)
	local c = ComponentTimer.create()
	c:add_callback(
		delay, 
		"ColourModulation.set_strength(1.2," .. tostring(1.2/fadeout) .. ", 0.000) " ..
		"Fog.set_strength(0, " .. tostring(0.00165/fadeout) .. ", 0.00165) " ..
		"this:get_component(ComponentDirectionalLight):set_strength(1.0, " .. tostring(1.0 / fadeout) .. ", 0)" 
	)
	c:add_callback(
		delay + fadeout, 
		"this:kill()"
	)
	e:add_component(c)
	local d = ComponentDirectionalLight.create(1.0, 0, -1.0, 1.0, 1.0, 1.0)
	e:add_component(d)
	pool:add(e)
end