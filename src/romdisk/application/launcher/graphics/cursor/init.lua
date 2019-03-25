local Pattern = Pattern

local M = Class(DisplayObject)

function M:init(w, h)
	self.super:init()

	local assets = assets

	self:addChild(DisplayShape.new(w, h)
		:setSource(Pattern.image(assets:loadImage("graphics/cursor/bg.png")):setExtend(Pattern.EXTEND_REPEAT))
		:paint())

	local cursor = assets:loadDisplay("graphics/cursor/cursor.png")
		:addEventListener(Event.MOUSE_DOWN, function(d, e) d:setPosition(e.x, e.y) end)
		:addEventListener(Event.MOUSE_MOVE, function(d, e) d:setPosition(e.x, e.y) end)
		:addEventListener(Event.MOUSE_UP, function(d, e) d:setPosition(e.x, e.y) end)
		:addEventListener(Event.TOUCH_BEGIN, function(d, e) d:setPosition(e.x, e.y) end)
		:addEventListener(Event.TOUCH_MOVE, function(d, e) d:setPosition(e.x, e.y) end)
		:addEventListener(Event.TOUCH_END, function(d, e) d:setPosition(e.x, e.y) end)
	self:addChild(cursor)
end

return M
