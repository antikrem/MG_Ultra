local cText = this:get_component(ComponentText)

local profileInfo = getProfileInfo()

update_global_silce(profileInfo)

g_d_profiler_currentSampleCount = g_d_profiler_currentSampleCount + 1

if g_d_profiler_currentSampleCount % g_d_profiler_samples == 0 then
	local profileString = get_dump_report() .. "\n"

	for system, values in pairs(g_d_profiler_slice) do
		profileString = 
			profileString 
			.. convertToProfileString(
				system, arrays.multiply(1/g_d_profiler_samples, values)
			)
	end

	cText:set_text(profileString)
	g_d_profiler_slice = {}
end

--emit an event to update the profiler for next time
emit_event(EventProfileUpdate)