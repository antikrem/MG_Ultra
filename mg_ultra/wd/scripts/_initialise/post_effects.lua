--\scripts\_initialise\post_effects.lua

--Global accessor for controlling post effects
PostEffects = {}

--Interface for settings values regarding exposure when creating the final image
PostEffects.Exposure = {}

--Exposure utilises a seperate target and current values, and a rate of change
--These values can be set here
PostEffects.Exposure.set_target = function(target)
	setExposureTarget(target)
end

PostEffects.Exposure.set_rate = function(rate)
	setExposureRate(rate)
end

--Sets exposure directly
PostEffects.Exposure.set = function(current)
	setExposure(current)
end

--Interface for setting bloom values
PostEffects.Bloom = {}

--sets the luminance threshold for bloom to be
PostEffects.Bloom.set_threshold = function(current)
	setBloomThreshold(current)
end

--Specifies far bloom's blur will effect, larger is wider
PostEffects.Bloom.set_deviation = function(current)
	setBloomDeviation(current)
end