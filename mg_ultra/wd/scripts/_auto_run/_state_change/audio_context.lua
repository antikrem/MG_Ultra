-- \scripts\_auto_run\_state_change\audio_context.lua

-- Work out what the target sound folder is
target = "campaigns\\" .. GlobalRegistar.get("next_campaign") .. "\\" .. GlobalRegistar.get("next_level")  ..  "\\sounds\\"

Audio.change_audio_load_level(target, Audio.LoadLevel.LEVEL)