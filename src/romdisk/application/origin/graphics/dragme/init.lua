local Pattern = Pattern

local M = Class(DisplayObject)

function M:init(w, h)
	self.super:init(w, h)

	local assets = assets

	self:addChild(DisplayShape.new(w, h)
		:setSource(Pattern.image(assets:loadImage("graphics/dragme/bg.png")):setExtend("repeat"))
		:paint())

	for i = 1, 5 do
		local shape = DisplayShape.new(100, 50)
			:setLineWidth(6)
			:rectangle(0, 0, 100, 50)
			:setSourceColor(1, 0, 0, 0.5)
			:fillPreserve()
			:setSourceColor(0, 0, 0)
			:stroke()
			:setPosition(math.random(0, w - 100), math.random(0, h - 50))
	
		shape:addEventListener("mouse-down", self.onMouseDown)
		shape:addEventListener("mouse-move", self.onMouseMove)
		shape:addEventListener("mouse-up", self.onMouseUp)
		shape:addEventListener("touch-begin", self.onTouchBegin)
		shape:addEventListener("touch-move", self.onTouchMove)
		shape:addEventListener("touch-end", self.onTouchEnd)
	
		self:addChild(shape)
	end
end

function M:onMouseDown(e)
	if self:hitTestPoint(e.x, e.y) then
		self.touchid = -1
		self.x0 = e.x
		self.y0 = e.y
		e.stop = true
	end
end

function M:onMouseMove(e)
	if self.touchid == -1 then
		local x1, y1 = self:globalToLocal(self.x0, self.y0)
		local x2, y2 = self:globalToLocal(e.x, e.y)
		self:setX(self:getX() + x2 - x1)
		self:setY(self:getY() + y2 - y1)
		self.x0 = e.x
		self.y0 = e.y
		e.stop = true
	end
end

function M:onMouseUp(e)
	if self.touchid == -1 then
		self.touchid = nil
		e.stop = true
	end
end

function M:onTouchBegin(e)
	if self:hitTestPoint(e.x, e.y) then
		self.touchid = e.id
		self.x0 = e.x
		self.y0 = e.y
		e.stop = true
	end
end

function M:onTouchMove(e)
	if self.touchid == e.id then
		local x1, y1 = self:globalToLocal(self.x0, self.y0)
		local x2, y2 = self:globalToLocal(e.x, e.y)
		self:setX(self:getX() + x2 - x1)
		self:setY(self:getY() + y2 - y1)
		self.x0 = e.x
		self.y0 = e.y
		e.stop = true
	end
end

function M:onTouchEnd(e)
	if self.touchid == e.id then
		self.touchid = nil
		e.stop = true
	end
end

return M
