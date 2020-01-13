--\scripts\_initialise\weather.lua

WeatherTypes = {}
WeatherTypes.None = "None"
WeatherTypes.HeavyRain = "HeavyRain"

--Current weather is saved as a string
registar:add("weather_type", WeatherTypes.None)

--particles used for weather
Particles.add_new_type("weather_heavyrain", "weather_rain0")
Particles.set_box_dimension("weather_heavyrain", 400, 400, 400)
Particles.set_box_center("weather_heavyrain", 0, 0, 400)
Particles.set_weight("weather_heavyrain", 0.00, 0.00)
Particles.set_type_response("weather_heavyrain", ParticleBoxResponse.Wrap)

--Assets for weather
Audio.request_load_file("rain", "rain.wav")

Weather = {}

--starts heavy rain
Weather.start_heavy_rain = function()
	registar:update("weather_type", WeatherTypes.HeavyRain)
	EntityPool.get_cached_entity(EntityWeather):get_component(ComponentParticle):spawn_uniformly(500)
	Drift.set_wind_speed(-5, -15, 0)
	Audio.play_track("rain")
end