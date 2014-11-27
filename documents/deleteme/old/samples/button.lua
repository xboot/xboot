local M = Class(DisplayObject)

function M:init(normal, active)
	DisplayObject.init(self)

	self.normal = normal
	self.active = active
	self.focus = false

	self:addChild(self.normal)
	self:addChild(self.active)
	self:updateVisualState(self.focus)

	self:addEventListener(Event.MOUSE_DOWN, self.onMouseDown, self)
	self:addEventListener(Event.MOUSE_MOVE, self.onMouseMove, self)
	self:addEventListener(Event.MOUSE_UP, self.onMouseUp, self)

	self:addEventListener(Event.TOUCH_BEGIN, self.onTouchBegin, self)
	self:addEventListener(Event.TOUCH_MOVE, self.onTouchMove, self)
	self:addEventListener(Event.TOUCH_END, self.onTouchEnd, self)
	self:addEventListener(Event.TOUCH_CANCEL, self.onTouchCancel, self)
end

function M:onMouseDown(e)
	if self:hitTestPoint(e.info.x, e.info.y) then
		self.focus = true
		self:updateVisualState(self.focus)
		e:stopPropagation()
	end
end

function M:onMouseMove(e)
	if self.focus then
		if not self:hitTestPoint(e.info.x, e.info.y) then	
			self.focus = false
			self:updateVisualState(self.focus)
		end
		e:stopPropagation()
	end
end

function M:onMouseUp(e)
	if self.focus then
		self.focus = false
		self:updateVisualState(self.focus)
		self:dispatchEvent(Event.new("click"))
		e:stopPropagation()
	end
end

function M:onTouchBegin(e)
	if self:hitTestPoint(e.info.x, e.info.y) then
		self.focus = true
		self:updateVisualState(self.focus)
		e:stopPropagation()
	end
end

function M:onTouchMove(e)
	if self.focus then
		if not self:hitTestPoint(e.info.x, e.info.y) then	
			self.focus = false
			self:updateVisualState(self.focus)
		end
		e:stopPropagation()
	end
end

function M:onTouchEnd(e)
	if self.focus then
		self.focus = false
		self:updateVisualState(self.focus)
		self:dispatchEvent(Event.new("click"))
		e:stopPropagation()
	end
end

function M:onTouchCancel(e)
	if self.focus then
		self.focus = false;
		self:updateVisualState(self.focus)
		e:stopPropagation()
	end
end

function M:updateVisualState(state)
	if state then
		self.normal:setVisible(false)
		self.active:setVisible(true)
	else
		self.normal:setVisible(true)
		self.active:setVisible(false)
	end
end

return M
