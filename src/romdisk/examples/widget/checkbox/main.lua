local stage = application:getStage()
local width, height = application:getScreenSize()

-- Create background
stage:addChild(DisplayShape.new(width, height):setSourceColor(0.8, 0.8, 0.8):paint())

-- Create the CheckBox
local checkbox = Widget.CheckBox.new({x = 100, y = 100})
	:addEventListener("Change", function(d, e) print("CheckBox changed:", e.info.checked) end)
stage:addChild(checkbox)
