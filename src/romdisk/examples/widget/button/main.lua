local stage = application:getStage()
local width, height = application:getScreenSize()

-- Create background
stage:addChild(DisplayShape.new(width, height):setSourceColor(0.3, 0.3, 0.3):paint())

-- Create the button
local btn = Widget.Button.new({x = 100, y = 100, width = 100, height = 50})
stage:addChild(btn)

-- Register to "Click" event
btn:addEventListener("Click", function(d, e)
	print("Button click ...")
end, btn)
