local M = Class(DisplayObject)

function M:init(on, off)
	DisplayObject.init(self)

	self.on = on
	self.off = off
	self.ison = false
	self.focus = nil

	self:addChild(self.on)
	self:addChild(self.off)
	self:updateVisualState(self.ison)

	self:addEventListener(Event.MOUSE_DOWN, self.onMouseDown, self)
	self:addEventListener(Event.MOUSE_MOVE, self.onMouseMove, self)
	self:addEventListener(Event.MOUSE_UP, self.onMouseUp, self)

	self:addEventListener(Event.TOUCH_BEGIN, self.onTouchBegin, self)
	self:addEventListener(Event.TOUCH_MOVE, self.onTouchMove, self)
	self:addEventListener(Event.TOUCH_END, self.onTouchEnd, self)
	self:addEventListener(Event.TOUCH_CANCEL, self.onTouchCancel, self)
end

function M:onMouseDown(e)
	if self.focus == nil and self:hitTestPoint(e.info.x, e.info.y) then
		self.focus = 0
		self.ison = not self.ison
		self:updateVisualState(self.ison)
		self:dispatchEvent(Event.new("Toggled", {on = self.ison}))
		e:stopPropagation()
	end
end

function M:onMouseMove(e)
	if self.focus == 0 then
		if not self:hitTestPoint(e.info.x, e.info.y) then	
			self.focus = nil
		end
		e:stopPropagation()
	end
end

function M:onMouseUp(e)
	if self.focus == 0 then
		self.focus = nil
		e:stopPropagation()
	end
end

function M:onTouchBegin(e)
	if self.focus == nil and self:hitTestPoint(e.info.x, e.info.y) then
		self.focus = e.info.id
		self.ison = not self.ison
		self:updateVisualState(self.ison)
		self:dispatchEvent(Event.new("Toggled", {on = self.ison}))
		e:stopPropagation()
	end
end

function M:onTouchMove(e)
	if self.focus == e.info.id then
		if not self:hitTestPoint(e.info.x, e.info.y) then
			self.focus = nil
		end
		e:stopPropagation()
	end
end

function M:onTouchEnd(e)
	if self.focus == e.info.id then
		self.focus = nil
		e:stopPropagation()
	end
end

function M:onTouchCancel(e)
	if self.focus == e.info.id then
		self.focus = nil;
		e:stopPropagation()
	end
end

function M:updateVisualState(state)
	if state then
		self.on:setVisible(true)
		self.off:setVisible(false)
	else
		self.on:setVisible(false)
		self.off:setVisible(true)
	end
end

return M
