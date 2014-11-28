local M = Class(DisplayObject)

function M:init(n, x, y)
	self.super:init()

	local assets = application:getAssets()
	local block = assets:loadDisplay("games/2048/tile" .. n .. ".png"):setAnchor(0.5, 0.5)
	
	block:addEventListener(Event.MOUSE_DOWN, self.onMouseDown, block)
	block:addEventListener(Event.MOUSE_MOVE, self.onMouseMove, block)
	block:addEventListener(Event.MOUSE_UP, self.onMouseUp, block)
	block:addEventListener(Event.TOUCH_BEGIN, self.onTouchBegin, block)
	block:addEventListener(Event.TOUCH_MOVE, self.onTouchMove, block)
	block:addEventListener(Event.TOUCH_END, self.onTouchEnd, block)

	self:addChild(block)
end

function M:onMouseDown(e)
	if self:hitTestPoint(e.info.x, e.info.y) then
		self.touchid = -1
		self.x0 = e.info.x
		self.y0 = e.info.y
		e:stopPropagation()
	end
end

function M:onMouseMove(e)
	if self.touchid == -1 then	
		local dx = e.info.x - self.x0
		local dy = e.info.y - self.y0
		self:setX(self:getX() + dx)
		self:setY(self:getY() + dy)
		self.x0 = e.info.x
		self.y0 = e.info.y
		e:stopPropagation()
	end
end

function M:onMouseUp(e)
	if self.touchid == -1 then
		self.touchid = nil
		e:stopPropagation()
	end
end

function M:onTouchBegin(e)
	if self:hitTestPoint(e.info.x, e.info.y) then
		self.touchid = e.info.id
		self.x0 = e.info.x
		self.y0 = e.info.y
		e:stopPropagation()
	end
end

function M:onTouchMove(e)
	if self.touchid == e.info.id then
		local dx = e.info.x - self.x0
		local dy = e.info.y - self.y0
		self:setX(self:getX() + dx)
		self:setY(self:getY() + dy)
		self.x0 = e.info.x
		self.y0 = e.info.y
		e:stopPropagation()
	end
end

function M:onTouchEnd(e)
	if self.touchid == e.info.id then
		self.touchid = nil
		e:stopPropagation()
	end
end

return M
