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
	EntityPool.get_cached_entity(EntityWeather):get_component(ComponentParticle):spawn_uniformly(750)
	EntityPool.get_cached_entity(EntityWeather):get_component(ComponentAudio):play("rain")
	Drift.set_wind_speed(10, -24, 0)

	--Moody lights and fog
	AmbientLight.set_strength(0.01)
	Fog.set_colour(0.4, 0.4, 0.4)
	Fog.set_strength(0.00165)
	Fog.set_starting_depth(400)

	local p = EntityPool.get_player()
	if p then p:get_component(ComponentPointLight):set_parameters(0.00078, 0, 2) end

	PostEffects.Bloom.set_threshold(0.2)
end