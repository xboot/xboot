local stage = application:getStage()
local assets = application:getAssets()

local cursor = assets:loadDisplay("cursor.png")
	:addEventListener(Event.MOUSE_DOWN, function(d, e) d:setPosition(e.info.x, e.info.y) end)
	:addEventListener(Event.MOUSE_MOVE, function(d, e) d:setPosition(e.info.x, e.info.y) end)
	:addEventListener(Event.MOUSE_UP, function(d, e) d:setPosition(e.info.x, e.info.y) end)
	:addEventListener(Event.TOUCHES_BEGIN, function(d, e) d:setPosition(e.info.x, e.info.y) end)
	:addEventListener(Event.TOUCHES_MOVE, function(d, e) d:setPosition(e.info.x, e.info.y) end)
	:addEventListener(Event.TOUCHES_END, function(d, e) d:setPosition(e.info.x, e.info.y) end)
	:addEventListener(Event.TOUCHES_CANCEL, function(d, e) d:setPosition(e.info.x, e.info.y) end)

stage:addChild(cursor)
