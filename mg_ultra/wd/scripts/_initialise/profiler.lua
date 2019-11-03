local cText = this:get_component(ComponentText)

local profileInfo = getProfileInfo()

local profileString = get_dump_report() .. "\n"

for system, values in pairs(profileInfo) do
	profileString = profileString .. convertToProfileString(system, values)
end

cText:set_text(profileString)

--emit an event to update the profiler for next time
emit_event(EventProfileUpdate)