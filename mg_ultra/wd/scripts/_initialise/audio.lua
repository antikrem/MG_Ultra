--_initialise/audio.lua
--Sets up Audio interface, which allows loading of audio files
--and some general interactions

Audio = {}

--loads an audio file for usage
Audio.load_file = addAudioFile

--plays a track, replacing last track
Audio.play_track = function(track) 
	
end

--prints a small report on Audio state
Audio.print_report = printAudioReport