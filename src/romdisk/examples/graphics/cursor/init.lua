local M = Class(DisplayObject)

function M:init()
	self.super:init()

	local w, h = application:getScreenSize()
	local assets = application:getAssets()

	local w, h = application:getScreenSize()
	self:addChild(DisplayShape.new(w, h):setSourceColor(1, 1, 1):paint())

	local cursor = assets:loadDisplay("graphics/cursor/cursor.png")
		:addEventListener(Event.MOUSE_DOWN, function(d, e) d:setPosition(e.info.x, e.info.y) end)
		:addEventListener(Event.MOUSE_MOVE, function(d, e) d:setPosition(e.info.x, e.info.y) end)
		:addEventListener(Event.MOUSE_UP, function(d, e) d:setPosition(e.info.x, e.info.y) end)
		:addEventListener(Event.TOUCH_BEGIN, function(d, e) d:setPosition(e.info.x, e.info.y) end)
		:addEventListener(Event.TOUCH_MOVE, function(d, e) d:setPosition(e.info.x, e.info.y) end)
		:addEventListener(Event.TOUCH_END, function(d, e) d:setPosition(e.info.x, e.info.y) end)
		:addEventListener(Event.TOUCH_CANCEL, function(d, e) d:setPosition(e.info.x, e.info.y) end)
	self:addChild(cursor)
end

return M
