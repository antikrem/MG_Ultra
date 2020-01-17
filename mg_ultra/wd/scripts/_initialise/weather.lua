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
	registar:update("weather_type", WeatherTypes.HeavyRain)
	EntityPool.get_cached_entity(EntityWeather):get_component(ComponentParticle):spawn_uniformly(1000)
	EntityPool.get_cached_entity(EntityWeather):get_component(ComponentAudio):play("rain")
	Drift.set_wind_speed(14, -28, 0)
	AmbientLight.set_strength(0.01)
end