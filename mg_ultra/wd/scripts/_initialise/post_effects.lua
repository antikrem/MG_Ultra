--\scripts\_initialise\post_effects.lua

-- Global accessor for controlling post effects
PostEffects = {}

-- Interface for settings values regarding exposure when creating the final image
PostEffects.Exposure = {}

-- Exposure utilises a seperate target and current values, and a rate of change
-- These values can be set here
PostEffects.Exposure.set_target = function(target)
	setExposureTarget(target)
end

PostEffects.Exposure.set_rate = function(rate)
	setExposureRate(rate)
end

-- Sets exposure directly
PostEffects.Exposure.set = function(current)
	setExposure(current)
end

-- Interface for settings values regarding greyscale when creating the final image
PostEffects.Greyscale = {}

-- Greyscale utilises a seperate target and current values, and a rate of change
-- These values can be set here
PostEffects.Greyscale.set_target = function(target)
	setGreyScaleFactorTarget(target)
end

PostEffects.Greyscale.set_rate = function(rate)
	setGreyScaleFactorRate(rate)
end

-- Sets Greyscale directly
PostEffects.Greyscale.set = function(current)
	setGreyScaleFactor(current)
end

-- Interface for setting bloom values
PostEffects.Bloom = {}

-- Sets the luminance threshold for bloom to be
PostEffects.Bloom.set_threshold = function(current)
	setBloomThreshold(current)
end

-- Specifies far bloom's blur will effect, larger is wider
PostEffects.Bloom.set_deviation = function(current)
	setBloomDeviation(current)
end

-- Reset post effects
PostEffects.reset = function()
	PostEffects.Exposure.set_target(1.0)
	PostEffects.Exposure.set_rate(1.0)
	PostEffects.Exposure.set(1.0)
end