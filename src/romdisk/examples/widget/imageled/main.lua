-- Create background
local bg = DisplayImage.new(Texture.new("images/bg.png"))
stage:addChild(bg)

-- Create the led
local led = Widget.ImageLed.new("images/on.png", "images/off.png")
led:setPosition(100, 100)
stage:addChild(led)

-- Create a timer for flash
Timer.new(0.4, 0, function(t, e)
	led:setState(not led:getState())
	print("Image Led flash ", led:getState())
end)
