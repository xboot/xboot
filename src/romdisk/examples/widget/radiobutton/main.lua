local stage = application:getStage()
local width, height = application:getScreenSize()

-- Create background
stage:addChild(DisplayShape.new(width, height):setSourceColor(0.8, 0.8, 0.8):paint())

-- Create the RadioButton
local radiobutton = Widget.RadioButton.new({x = 100, y = 100})
	:addEventListener("Change", function(d, e) print("RadioButton changed:", e.info.checked) end)
stage:addChild(radiobutton)
