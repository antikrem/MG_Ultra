-- \scripts\_auto_run\_state_change\ui_change.lua

-- If new state is level, display UI
if GlobalRegistar.get("state") == "level" then
	UI.show_ui()
--Otherwise hide it
else
	UI.hide_ui()
end