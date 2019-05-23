local DisplayBmtext = require "DisplayBmtext"
local sw, sh = stage:getSize()

local background = assets:loadDisplay("background.png")
stage:addChild(background)

local bmtext = DisplayBmtext.new("hobostd.png", "hobostd.txt", "1234567890"):setPosition(100,100)
stage:addChild(bmtext)

local count = 0
stage:addTimer(Timer.new(0.01, 0, function(t)
	count = count + 0.001
	bmtext:setText("time: " .. string.format("%.3f", count))
end))

stage:addTimer(Timer.new(1, 0, function(t)
	local w, h = 200, 50
	local s = math.random() * 1.0 + 0.5
	bmtext:animate({x = math.random(0, sw - w), y = math.random(0, sh - h), scalex = s, scaley = s}, 1, "circ-out")
end))

