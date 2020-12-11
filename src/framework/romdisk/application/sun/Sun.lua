local M = Class(DisplayObject)

function M:init(x, y)
	self._x = x or 0
	self._y = y or 0
	self._sun = assets:loadDisplay("assets/images/sun.png")
	self.super:init(self._sun:getSize())

	self:addChild(self._sun)
	self:setAnchor(0.5, 0.5)
	self:setPosition(self._x, self._y)
	self:addEventListener("mouse-down", self.onMouseDown)
	self:addEventListener("mouse-move", self.onMouseMove)
	self:addEventListener("mouse-up", self.onMouseUp)
	self:addEventListener("touch-begin", self.onTouchBegin)
	self:addEventListener("touch-move", self.onTouchMove)
	self:addEventListener("touch-end", self.onTouchEnd)
end

function M:onMouseDown(e)
	if self:hitTestPoint(e.x, e.y) then
		self:spring()
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
		self:spring({x = self._x, y = self._y}, 0, 300, 7)
		self.touchid = nil
		e.stop = true
	end
end

function M:onTouchBegin(e)
	if self:hitTestPoint(e.x, e.y) then
		self:spring()
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
		self:spring({x = self._x, y = self._y}, 0, 300, 7)
		self.touchid = nil
		e.stop = true
	end
end

return M
