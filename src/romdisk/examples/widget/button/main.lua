local stage = application:getStage()
local width, height = application:getScreenSize()

-- Create background
stage:addChild(DisplayShape.new(width, height):setSourceColor(0.3, 0.3, 0.3):paint())

-- Create the button
local button = Widget.Button.new({x = 100, y = 100, width = 100, height = 50})
	:addEventListener("Press", function(d, e) print("Button [Press]") end)
	:addEventListener("Release", function(d, e) print("Button [Release]") end)
	:addEventListener("Click", function(d, e) print("Button [Click]") end)
stage:addChild(button)
