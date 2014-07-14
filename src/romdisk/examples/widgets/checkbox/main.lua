local stage = application:getStage()
local width, height = application:getScreenSize()

local checkbox = Widget.CheckBox.new({x = 100, y = 100})
	:addEventListener("Change", function(d, e) print("CheckBox changed:", e.info.checked) end)
stage:addChild(checkbox)
