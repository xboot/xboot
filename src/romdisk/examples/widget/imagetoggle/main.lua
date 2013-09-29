-- Create background
local bg = DisplayImage.new(Texture.new("images/bg.png"))
runtime:addChild(bg)

-- Create the toggle
local tog = Widget.ImageToogle.new("images/on.png", "images/off.png")
tog:setPosition(100, 100)
runtime:addChild(tog)

-- Register to "Toggle" event
tog:addEventListener("Toggle", function(d, e)
	print("Image toggle state ", e.info.state)
end, tog)
