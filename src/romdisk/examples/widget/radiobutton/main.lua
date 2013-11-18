local stage = application:getStage()
local width, height = application:getScreenSize()

-- Create background
stage:addChild(DisplayShape.new(width, height):setSourceColor(0.3, 0.3, 0.3):paint())

-- Create the radiobutton
local radiobutton = Widget.RadioButton.new({x = 100, y = 100})
stage:addChild(radiobutton)

-- Register to "Change" event
radiobutton:addEventListener("Change", function(d, e)
	print("RadioButton changed:", e.info.checked)
end, radiobutton)
