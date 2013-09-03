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

	self:addEventListener(Event.TOUCHES_BEGIN, self.onTouchesBegin, self)
	self:addEventListener(Event.TOUCHES_MOVE, self.onTouchesMove, self)
	self:addEventListener(Event.TOUCHES_END, self.onTouchesEnd, self)
	self:addEventListener(Event.TOUCHES_CANCEL, self.onTouchesCancel, self)
end

function M:onMouseDown(e)
	if self:hitTest(e.info.x, e.info.y) then
		self.focus = true
		self:updateVisualState(self.focus)
		e:stopPropagation()
	end
end

function M:onMouseMove(e)
	if self.focus then
		if not self:hitTest(e.info.x, e.info.y) then	
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
		self:dispatchEvent(Event:new("click"))
		e:stopPropagation()
	end
end

function M:onTouchesBegin(e)
	if self:hitTest(e.info.x, e.info.y) then
		self.focus = true
		self:updateVisualState(self.focus)
		e:stopPropagation()
	end
end

function M:onTouchesMove(e)
	if self.focus then
		if not self:hitTest(e.info.x, e.info.y) then	
			self.focus = false
			self:updateVisualState(self.focus)
		end
		e:stopPropagation()
	end
end

function M:onTouchesEnd(e)
	if self.focus then
		self.focus = false
		self:updateVisualState(self.focus)
		self:dispatchEvent(Event:new("click"))
		e:stopPropagation()
	end
end

function M:onTouchesCancel(e)
	if self.focus then
		self.focus = false;
		self:updateVisualState(self.focus)
		e:stopPropagation()
	end
end

function M:updateVisualState(state)
	if state then
		self.normal:visible(false)
		self.active:visible(true)
		self.width = self.active.width
		self.height = self.active.height
	else
		self.normal:visible(true)
		self.active:visible(false)
		self.width = self.normal.width
		self.height = self.normal.height
	end
end

return M
