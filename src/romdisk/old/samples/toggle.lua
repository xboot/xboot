local M = Class(DisplayObject)

function M:init(on, off)
	DisplayObject.init(self)

	self.on = on
	self.off = off
	self.ison = false
	self.focus = false

	self:addChild(self.on)
	self:addChild(self.off)
	self:updateVisualState(self.ison)

	self:addEventListener(Event.MOUSE_DOWN, self.onMouseDown, self)
	self:addEventListener(Event.MOUSE_MOVE, self.onMouseMove, self)
	self:addEventListener(Event.MOUSE_UP, self.onMouseUp, self)

	self:addEventListener(Event.TOUCHES_BEGIN, self.onTouchesBegin, self)
	self:addEventListener(Event.TOUCHES_MOVE, self.onTouchesMove, self)
	self:addEventListener(Event.TOUCHES_END, self.onTouchesEnd, self)
	self:addEventListener(Event.TOUCHES_CANCEL, self.onTouchesCancel, self)
end

function M:onMouseDown(e)
	if self:hitTestPoint(e.info.x, e.info.y) then
		self.focus = true
		self.ison = not self.ison
		self:updateVisualState(self.ison)
		self:dispatchEvent(Event:new("toggled"))
		e:stopPropagation()
	end
end

function M:onMouseMove(e)
	if self.focus then
		if not self:hitTestPoint(e.info.x, e.info.y) then	
			self.focus = false
		end
		e:stopPropagation()
	end
end

function M:onMouseUp(e)
	if self.focus then
		self.focus = false
		e:stopPropagation()
	end
end

function M:onTouchesBegin(e)
	if self:hitTestPoint(e.info.x, e.info.y) then
		self.focus = true
		self.ison = not self.ison
		self:updateVisualState(self.ison)
		self:dispatchEvent(Event:new("toggled"))
		e:stopPropagation()
	end
end

function M:onTouchesMove(e)
	if self.focus then
		if not self:hitTestPoint(e.info.x, e.info.y) then	
			self.focus = false
		end
		e:stopPropagation()
	end
end

function M:onTouchesEnd(e)
	if self.focus then
		self.focus = false
		e:stopPropagation()
	end
end

function M:onTouchesCancel(e)
	if self.focus then
		self.focus = false;
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
