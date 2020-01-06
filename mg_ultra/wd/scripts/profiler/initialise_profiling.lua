--\scripts\profiler\pinitialise_profiling.lua

----Some registar values for profiling
--Set to true when profiling is active
registar:add("d_profiler_active", false)
--Number of samples for profiling
g_d_profiler_samples = 10
--retained slice, used for averaging:
g_d_profiler_slice = {}
--counter used for determining number of samples
g_d_profiler_currentSampleCount = g_d_profiler_samples - 1


--A function for starting the Profiler
function start_profiling()
	registar:update("d_profiler_active", true)
end

--Stop profiling
function stop_profiling()
	registar:update("d_profiler_active", false)
end

--Updates profile information
function update_global_silce(slice)
	for sName, values in pairs(slice) do
		if is_nil(g_d_profiler_slice[sName]) then
			g_d_profiler_slice[sName] = {0, 0, 0}
		end
		g_d_profiler_slice[sName] = arrays.sum(g_d_profiler_slice[sName], values)
	end

end
--convert a profile slice into a string of information
function convertToProfileString(name, slice)
	if slice[3] < 1 then
		--If theres a timing, we want to display :
		-- 1. proportion of period spent executinf
		-- 2. effective timing,
		return 
			string.pad_string(name, 12)
			.. "F-> "
			.. string.format("exec: %.3f ", slice[2]/slice[1]) 
			.. string.pad_string(string.format("etime: %d", math.floor(1000000 / (slice[2] + 1))), 15)
			.. "\n"
	else 
		--If theres a timing, we want to display :
		-- 1. proportion of period spent executing
		-- 2. ratio of period to timing, as a percentage
		local desiredTimePerCycle = 1000000 / slice[3]
		return 
			string.pad_string(name, 12)
			.. "T-> "
			.. string.format("exec: %.3f ", slice[2]/slice[1]) 
			.. string.pad_string(string.format("speed: %3.2f%% ", 100*(slice[1] / desiredTimePerCycle) ), 15)
			.. "\n"

	end
end

--generates a small string of all distinguished masters
function reportDistinguishedMaster()
	return 
		"Distinguished Masters:\n"
		.. "ScriptMaster: count: " .. string.pad_sub(tostring(getScriptMasterLastCalls()), 5)
		.. " last: " .. string.pad_sub(tostring(getScriptMasterLastFPS()), 5)
		.. "\nGLHandler: count: " .. string.pad_sub(tostring(getGLHandlerLastCalls()), 5)
		.. " last: " .. string.pad_sub(tostring(getGLHandlerLastFPS()), 5)
end

