local Button = require("Button")

-- create background
local background = DisplayImage:new(Texture.new("background.png"))
runtime:addChild(background)

-- create the up and down sprites for the button
local normal = DisplayImage:new(Texture.new("button_normal.png"))
local active = DisplayImage:new(Texture.new("button_active.png"))

-- create the button
local btn = Button:new(normal, active)

-- register to "click" event
local click = 0
btn:addEventListener("Click",
	function(d, e)
		click = click + 1
		print("Clicked " .. click .. " times")
	end, btn)

btn:setPosition(100, 100)
runtime:addChild(btn)