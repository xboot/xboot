local Pattern = Pattern

local cursor = assets:loadDisplay("cursor.png")
	:addEventListener(Event.MOUSE_DOWN, function(d, e) d:setPosition(e.x, e.y) end)
	:addEventListener(Event.MOUSE_MOVE, function(d, e) d:setPosition(e.x, e.y) end)
	:addEventListener(Event.MOUSE_UP, function(d, e) d:setPosition(e.x, e.y) end)
	:addEventListener(Event.TOUCH_BEGIN, function(d, e) d:setPosition(e.x, e.y) end)
	:addEventListener(Event.TOUCH_MOVE, function(d, e) d:setPosition(e.x, e.y) end)
	:addEventListener(Event.TOUCH_END, function(d, e) d:setPosition(e.x, e.y) end)

stage:addChild(cursor)
stage:showfps(true)
