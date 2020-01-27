--\scripts\_initialise\audio.lua

--Sets up Audio interface, which allows loading of audio files
--and some general interactions
Audio = {}

--loads an audio file for usage
--does not load instantly
Audio.request_load_file = addAudioFile

--flushses the load request queue
Audio.flush_queue = flushAudioLoadRequests

--adds a file and waits for it to have been processed
Audio.load_file = function(audioName, fileLocation)
	Audio.request_load_file(audioName, fileLocation)
	Audio.flush_queue()
end

--prints a small report on Audio state
Audio.print_report = printAudioReport

--Global music player
Music = {}

--Music global gain
Music.globalGain = 1.0

--Music last gain
Music.lastGain = 1.0

--plays a music track, replacing last track
Music.play_track = function(track)
	local musicPlayer = EntityPool.get_cached_entity(EntityMusic)
	
	if not is_nil(musicPlayer) then
		local cAudio = musicPlayer:get_component(ComponentAudio)
		cAudio:play(track)
		Music.lastGain = 1.0
		Music.evaluateNewGain()
	else
		print("MUSIC, Error playing track " .. track .. ", music player was not found")
	end
	
end

Music.evaluateNewGain = function()
	local musicPlayer = EntityPool.get_cached_entity(EntityMusic)
	
	if not is_nil(musicPlayer) then
		local cAudio = musicPlayer:get_component(ComponentAudio)
		cAudio:set_gain(Music.lastGain * Music.globalGain)
	else
		print("MUSIC, Error managing gain " .. track .. ", music player was not found")
	end
end

Music.set_gain = function(strength)
	Music.lastGain = strength
	Music.evaluateNewGain()
end

Music.set_global_modifier = function(strength)
	Music.globalGain = strength
	Music.evaluateNewGain()
end