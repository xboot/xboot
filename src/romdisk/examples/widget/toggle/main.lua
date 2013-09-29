-- Create background
local bg = DisplayImage.new(Texture.new("images/bg.png"))
runtime:addChild(bg)

-- Create up and down display object for the toggle
local on = DisplayImage.new(Texture.new("images/on.png"))
local off = DisplayImage.new(Texture.new("images/off.png"))

-- Create the toggle
local tog = Widget.Toggle.new(on, off)
tog:setPosition(100, 100)
runtime:addChild(tog)

-- Register to "Toggle" event
tog:addEventListener("Toggle", function(d, e)
	print("Toggle state ", e.info.state)
end, tog)
