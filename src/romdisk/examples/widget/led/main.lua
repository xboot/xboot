-- Create background
local bg = DisplayImage.new(Texture.new("images/bg.png"))
runtime:addChild(bg)

-- Create up and down display object for the led
local on = DisplayImage.new(Texture.new("images/on.png"))
local off = DisplayImage.new(Texture.new("images/off.png"))

-- Create the led
local led = Widget.Led.new(on, off)
led:setPosition(100, 100)
runtime:addChild(led)

-- Create a timer for flash
Timer.new(0.4, 0, function(t, e)
	led:setState(not led:getState())
	print("Led flash ", led:getState())
end)
