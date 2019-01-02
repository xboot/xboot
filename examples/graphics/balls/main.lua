local Ball = require "Ball"

local maxWidth, maxHeight = stage:getSize()

for i = 1, 5 do
	stage:addChild(Ball.new(maxWidth, maxHeight):setAnchor(0.5, 0.5))
end

stage:addEventListener(Event.MOUSE_UP, function(d, e) stage:addChild(Ball.new(maxWidth, maxHeight):setPosition(e.x, e.y):setAnchor(0.5, 0.5)) end)
	:addEventListener(Event.TOUCH_END, function(d, e) stage:addChild(Ball.new(maxWidth, maxHeight):setPosition(e.x, e.y):setAnchor(0.5, 0.5)) end)
stage:showfps(true)
