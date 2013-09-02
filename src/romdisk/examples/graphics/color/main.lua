local background = display_image:new("background.png")
runtime:add_child(background)

local redbar = display_image:new("red-bar.png")
redbar:setxy(91, 230)
runtime:add_child(redbar)

local greenbar = display_image:new("green-bar.png")
greenbar:setxy(91, 260)
runtime:add_child(greenbar)

local bluebar = display_image:new("blue-bar.png")
bluebar:setxy(91, 290)
runtime:add_child(bluebar)

local reddot = display_image:new("red-dot.png")
reddot:sety(222)
runtime:add_child(reddot)

local greendot = display_image:new("green-dot.png")
greendot:sety(252)
runtime:add_child(greendot)

local bluedot = display_image:new("blue-dot.png")
bluedot:sety(282)
runtime:add_child(bluedot)

local c = 1
local function onEnterFrame(d, e)
	-- get the precise timer
	c = c + 1
	local t = c / 10
	
	-- animate r,g,b multipliers of color transform
	local r = (math.sin(t * 0.5 + 0.3) + 1) / 2
	local g = (math.sin(t * 0.8 + 0.2) + 1) / 2
	local b = (math.sin(t * 1.3 + 0.6) + 1) / 2
	
	-- set color transform
--	bitmap:setColorTransform(r, g, b, 1)

	-- update the positions of dots
	reddot:setx(90 + r * 280)
	greendot:setx(90 + g * 280)
	bluedot:setx(90 + b * 280)
end

runtime:add_event_listener(event.ENTER_FRAME, onEnterFrame)

