local cText = this:get_component(ComponentText)

local profileInfo = getProfileInfo()

local profileString = ""

for system, values in pairs(profileInfo) do
	profileString = profileString .. system .. " " .. tostring(values[1])  .. "\n"
end

cText:set_text(profileString)

--emit an event to update the profiler for next time
emit_event(EventProfileUpdate)