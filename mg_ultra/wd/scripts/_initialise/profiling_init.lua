print("Profiler")

--A function for starting the Profiler
function start_profiling()
	registar:update("profiler_active", true)
end

--Stop profiling
function stop_profiling()
	registar:update("profiler_active", false)
end