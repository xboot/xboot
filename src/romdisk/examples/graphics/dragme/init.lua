local M = Class(DisplayObject)

function M:init()
	self.super:init()

	local w, h = application:getScreenSize()
	self:addChild(DisplayShape.new(w, h):setSourceColor(0.8, 0.7, 0.5):paint())

	for i = 1, 5 do
		local shape = DisplayShape.new(100, 50)
			:setLineWidth(6)
			:rectangle(0, 0, 100, 50)
			:setSourceColor(1, 0, 0, 0.5)
			:fillPreserve()
			:setSourceColor(0, 0, 0)
			:stroke()
			:setPosition(math.random(0, w - 100), math.random(0, h - 50))
	
		shape:addEventListener(Event.MOUSE_DOWN, self.onMouseDown, shape)
		shape:addEventListener(Event.MOUSE_MOVE, self.onMouseMove, shape)
		shape:addEventListener(Event.MOUSE_UP, self.onMouseUp, shape)
		shape:addEventListener(Event.TOUCH_BEGIN, self.onTouchBegin, shape)
		shape:addEventListener(Event.TOUCH_MOVE, self.onTouchMove, shape)
		shape:addEventListener(Event.TOUCH_END, self.onTouchEnd, shape)
		shape:addEventListener(Event.TOUCH_CANCEL, self.onTouchCancel, shape)
	
		self:addChild(shape)
	end
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

function M:onTouchCancel(e)
	if self.touchid == e.info.id then
		self.touchid = nil
		e:stopPropagation()
	end
end

return M
