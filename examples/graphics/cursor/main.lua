local Pattern = Pattern

local cursor = assets:loadDisplay("cursor.png")
	:addEventListener("mouse-down", function(d, e) d:setPosition(e.x, e.y) end)
	:addEventListener("mouse-move", function(d, e) d:setPosition(e.x, e.y) end)
	:addEventListener("mouse-up", function(d, e) d:setPosition(e.x, e.y) end)
	:addEventListener("touch-begin", function(d, e) d:setPosition(e.x, e.y) end)
	:addEventListener("touch-move", function(d, e) d:setPosition(e.x, e.y) end)
	:addEventListener("touch-end", function(d, e) d:setPosition(e.x, e.y) end)

stage:addChild(cursor)
