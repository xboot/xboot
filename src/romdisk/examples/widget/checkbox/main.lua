local stage = application:getStage()
local width, height = application:getScreenSize()

-- Create background
stage:addChild(DisplayShape.new(width, height):setSourceColor(0.3, 0.3, 0.3):paint())

-- Create the checkbox
local checkbox = Widget.CheckBox.new({x = 100, y = 100})
stage:addChild(checkbox)

-- Register to "Change" event
checkbox:addEventListener("Change", function(d, e)
	print("Checkbox changed:", e.info.checked)
end, checkbox)
