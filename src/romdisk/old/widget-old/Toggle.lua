local M = Class(DisplayObject)

function M:init(on, off, state)
	self.super:init()

	self.on = assert(on)
	self.off = assert(off)
	self.state = type(state) == "boolean" and state or false
	self.focus = nil
	self:updateVisualState(self.state)

	self:addEventListener(Event.MOUSE_DOWN, self.onMouseDown, self)
	self:addEventListener(Event.MOUSE_MOVE, self.onMouseMove, self)
	self:addEventListener(Event.MOUSE_UP, self.onMouseUp, self)

	self:addEventListener(Event.TOUCHES_BEGIN, self.onTouchesBegin, self)
	self:addEventListener(Event.TOUCHES_MOVE, self.onTouchesMove, self)
	self:addEventListener(Event.TOUCHES_END, self.onTouchesEnd, self)
	self:addEventListener(Event.TOUCHES_CANCEL, self.onTouchesCancel, self)
end

function M:setState(state)
	if type(state) == "boolean" and self.state ~= state then
		self.state = state
		self:updateVisualState(self.state)
	end
end

function M:getState()
	return self.state
end

function M:onMouseDown(e)
	if self.focus == nil and self:hitTestPoint(e.info.x, e.info.y) then
		self.focus = 0
		self.state = not self.state
		self:updateVisualState(self.state)
		self:dispatchEvent(Event.new("Toggle", {on = self.state}))
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

function M:onTouchesBegin(e)
	if self.focus == nil and self:hitTestPoint(e.info.x, e.info.y) then
		self.focus = e.info.id
		self.state = not self.state
		self:updateVisualState(self.state)
		self:dispatchEvent(Event.new("Toggle", {state = self.state}))
		e:stopPropagation()
	end
end

function M:onTouchesMove(e)
	if self.focus == e.info.id then
		if not self:hitTestPoint(e.info.x, e.info.y) then
			self.focus = nil
		end
		e:stopPropagation()
	end
end

function M:onTouchesEnd(e)
	if self.focus == e.info.id then
		self.focus = nil
		e:stopPropagation()
	end
end

function M:onTouchesCancel(e)
	if self.focus == e.info.id then
		self.focus = nil
		e:stopPropagation()
	end
end

function M:updateVisualState(state)
	if state then
		if self:contains(self.off) then
			self:removeChild(self.off)
		end
		if not self:contains(self.on) then
			self:addChild(self.on)
		end
	else
		if self:contains(self.on) then
			self:removeChild(self.on)
		end
		if not self:contains(self.off) then
			self:addChild(self.off)
		end
	end
end

return M
