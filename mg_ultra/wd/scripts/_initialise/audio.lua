-- \scripts\_initialise\audio.lua

-- Enumeration of states an audio state can be in
AudioState = {}
AudioState.NONE = -1
AudioState.STOP = 0
AudioState.PLAY = 1 
AudioState.PAUSE = 2 

-- Sets up Audio interface, which allows loading of audio files
-- and some general interactions
Audio = {}

-- loads an audio file for usage
-- does not load instantly
Audio.request_load_file = addAudioFile

-- flushses the load request queue
Audio.flush_queue = flushAudioLoadRequests

-- adds a file and waits for it to have been processed
Audio.load_file = function(audioName, fileLocation)
	Audio.request_load_file(audioName, fileLocation)
	Audio.flush_queue()
end

-- prints a small report on Audio state
Audio.print_report = printAudioReport

-- Plays a single audio once
Audio.play_once = function(audioName)
	local e = Entity.create(EntityAnonymousSoundSource)
	e:add_component(ComponentAudio.create(audioName, true))
	EntityPool.add_entity(e)
end

-- Plays a single audio once
Audio.stop_playing_once = function()
	EntityPool.kill_by_type(EntityAnonymousSoundSource)
end

-- Global music player
Music = {}

-- Music global gain
Music.globalGain = 1.0

-- Music last gain, used for only last track
-- and reset on new track
Music.lastGain = 1.0

-- Plays a music track, replacing last track
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

-- Pause toggle on music
Music.toggle_pause = function()
	local musicPlayer = EntityPool.get_cached_entity(EntityMusic)
	
	if not is_nil(musicPlayer) then
		local cAudio = musicPlayer:get_component(ComponentAudio)
		if cAudio:get_state() == AudioState.PLAY then
			cAudio:pause()
		elseif cAudio:get_state() == AudioState.PAUSE then
			cAudio:resume()
		end
	else
		print("MUSIC, Error pausing music, music player was not found")
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