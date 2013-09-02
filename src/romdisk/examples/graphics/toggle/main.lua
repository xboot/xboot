local toggle = require("toggle")

-- create background
local background = display_image:new("background.png")
runtime:add_child(background)

-- create the up and down sprites for the button
local on = display_image:new("toggle-on.png")
local off = display_image:new("toggle-off.png")
	
-- create the toggle
local tog = toggle:new(on, off)

-- register to "toggled" event
tog:add_event_listener("toggled",
	function(d, e)
		print("toggle click ", e.info.on)
	end, tog)
	
tog:setxy(100, 100)
runtime:add_child(tog)
