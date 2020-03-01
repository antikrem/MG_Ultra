-- \scripts\_auto_run\_on_cycle\ui_update.lua
-- Updates the ui on cycle

-- Create UI if not existent
if not UI.uiCreated then
	UI.start_ui()
end

-- If the ui is visible, update it
if UI.uiVisible then
	UI.update_ui()
end