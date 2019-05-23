local Ball = require "Ball"

local maxWidth, maxHeight = stage:getSize()

for i = 1, 5 do
	stage:addChild(Ball.new(maxWidth, maxHeight))
end

stage:addEventListener("mouse-up", function(d, e) stage:addChild(Ball.new(maxWidth, maxHeight):setPosition(e.x, e.y)) end)
	:addEventListener("touch-end", function(d, e) stage:addChild(Ball.new(maxWidth, maxHeight):setPosition(e.x, e.y)) end)
