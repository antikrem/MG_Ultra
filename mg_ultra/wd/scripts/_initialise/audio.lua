--_initialise/audio.lua
--Sets up Audio interface, which allows loading of audio files
--and some general interactions

Audio = {}

--loads an audio file for usage
Audio.load_file = addAudioFile

--plays a track, replacing last track
Audio.play_track = function(track) 
	local musicPlayer = EntityPool.get_cached_entity(EntityMusic)
	
	if not is_nil(musicPlayer) then
		local cAudio = musicPlayer:get_component(ComponentAudio)
		cAudio:play(track)
	else 
		print("AUDIO, Error playing track " .. track .. ", music player was not found")
	end
	
end

--prints a small report on Audio state
Audio.print_report = printAudioReport