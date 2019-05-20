local Pattern = Pattern

local M = Class(DisplayObject)

function M:init(w, h)
	self.super:init(w, h)

	local assets = assets

	self:addChild(DisplayShape.new(w, h)
		:setSource(Pattern.image(assets:loadImage("graphics/cursor/bg.png")):setExtend("repeat"))
		:paint())

	local cursor = assets:loadDisplay("graphics/cursor/cursor.png")
		:addEventListener("mouse-down", function(d, e) d:setPosition(e.x, e.y) end)
		:addEventListener("mouse-move", function(d, e) d:setPosition(e.x, e.y) end)
		:addEventListener("mouse-up", function(d, e) d:setPosition(e.x, e.y) end)
		:addEventListener("touch-begin", function(d, e) d:setPosition(e.x, e.y) end)
		:addEventListener("touch-move", function(d, e) d:setPosition(e.x, e.y) end)
		:addEventListener("touch-end", function(d, e) d:setPosition(e.x, e.y) end)
	self:addChild(cursor)
end

return M
