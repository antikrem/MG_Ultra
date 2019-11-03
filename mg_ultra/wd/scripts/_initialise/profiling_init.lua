print("Profiler")

--A function for starting the Profiler
function start_profiling()
	registar:update("profiler_active", true)
end

--Stop profiling
function stop_profiling()
	registar:update("profiler_active", false)
end

--convert a profile slice into a string of information
function convertToProfileString(name, slice)
	if slice[3] < 1 then
		--If theres a timing, we want to display :
		-- 1. proportion of period spent executinf
		-- 2. effective timing,
		return name .. ": " 
			.. "Float "
			.. string.format("exec: %.3f ", slice[2]/slice[1]) 
			.. string.format("etime: %d", math.floor(1000000 / (slice[2] + 1))) 
			.. "\n"
	else 
		--If theres a timing, we want to display :
		-- 1. proportion of period spent executing
		-- 2. ratio of period to timing, as a percentage
		local desiredTimePerCycle = 1000000 / slice[3]
		return name .. ": " 
			.. "Timed "
			.. string.format("exec: %.3f ", slice[2]/slice[1]) 
			.. string.format("speed: %.2f%% ", clamp(100*(desiredTimePerCycle/slice[2]), 0, 100)) 
			.. "\n"

	end
end