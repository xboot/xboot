-- Create background
local bg = DisplayImage.new(Texture.new("images/bg.png"))
stage:addChild(bg)

-- Create the toggle
local tog = Widget.ImageToogle.new("images/on.png", "images/off.png")
tog:setPosition(100, 100)
stage:addChild(tog)

-- Register to "toggle" event
tog:addEventListener("toggle", function(d, e)
	print("Image toggle state ", e.info.state)
end, tog)
