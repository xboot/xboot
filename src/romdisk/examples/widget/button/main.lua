-- Create background
local background = DisplayImage.new(asset:loadTexture("assets/images/background.png"))
stage:addChild(background)

-- Create the button
local button = Widget.Button.new({x = 100, y = 100, width = 100, height = 50})
stage:addChild(button)

-- Register to "Click" event
local count = 0
button:addEventListener("Click", function(d, e)
	count = count + 1
	print("Button clicked " .. count .. " times")
end, button)
