local button = require("button")

-- create background
local background = display_image:new("background.png")
runtime:add_child(background)

-- create the up and down sprites for the button
local normal = display_image:new("button_normal.png")
local active = display_image:new("button_active.png")

-- create the button
local btn = button:new(normal, active)

-- register to "click" event
local click = 0
btn:add_event_listener("click",
	function(d, e)
		click = click + 1
		print("Clicked " .. click .. " times")
	end, btn)

btn:setxy(100, 100)
runtime:add_child(btn)