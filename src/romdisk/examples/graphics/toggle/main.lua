local Toggle = require("Toggle")

-- create background
local background = DisplayImage.new(Texture.new("background.png"))
runtime:addChild(background)

-- create the up and down sprites for the button
local on = DisplayImage.new(Texture.new("toggle-on.png"))
local off = DisplayImage.new(Texture.new("toggle-off.png"))
	
-- create the toggle
local tog = Toggle.new(on, off)

-- register to "toggled" event
tog:addEventListener("Toggled",
	function(d, e)
		print("toggle click ", e.info.on)
	end, tog)
	
tog:setPosition(100, 100)
runtime:addChild(tog)
