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

--plays a music track, replacing last track
Music.play_track = function(track)
	local musicPlayer = EntityPool.get_cached_entity(EntityMusic)
	
	if not is_nil(musicPlayer) then
		local cAudio = musicPlayer:get_component(ComponentAudio)
		cAudio:play(track)
	else
		print("AUDIO, Error playing track " .. track .. ", music player was not found")
	end
	
end