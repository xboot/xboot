local M = Class(DisplayObject)

function M:init(up, down)
	self.super:init()

	self.up = assert(up)
	self.down = assert(down)
	self.focus = nil
	self:updateVisualState(false)

	self:addEventListener(Event.MOUSE_DOWN, self.onMouseDown, self)
	self:addEventListener(Event.MOUSE_MOVE, self.onMouseMove, self)
	self:addEventListener(Event.MOUSE_UP, self.onMouseUp, self)

	self:addEventListener(Event.TOUCHES_BEGIN, self.onTouchesBegin, self)
	self:addEventListener(Event.TOUCHES_MOVE, self.onTouchesMove, self)
	self:addEventListener(Event.TOUCHES_END, self.onTouchesEnd, self)
	self:addEventListener(Event.TOUCHES_CANCEL, self.onTouchesCancel, self)
end

function M:onMouseDown(e)
	if self.focus == nil and self:hitTestPoint(e.info.x, e.info.y) then
		self.focus = 0
		self:updateVisualState(true)
		e:stopPropagation()
	end
end

function M:onMouseMove(e)
	if self.focus == 0 then
		if not self:hitTestPoint(e.info.x, e.info.y) then
			self.focus = nil
			self:updateVisualState(false)
		end
		e:stopPropagation()
	end
end

function M:onMouseUp(e)
	if self.focus == 0 then
		self.focus = nil
		self:updateVisualState(false)
		self:dispatchEvent(Event.new("Click"))
		e:stopPropagation()
	end
end

function M:onTouchesBegin(e)
	if self.focus == nil and self:hitTestPoint(e.info.x, e.info.y) then
		self.focus = e.info.id
		self:updateVisualState(true)
		e:stopPropagation()
	end
end

function M:onTouchesMove(e)
	if self.focus == e.info.id then
		if not self:hitTestPoint(e.info.x, e.info.y) then
			self.focus = nil
			self:updateVisualState(false)
		end
		e:stopPropagation()
	end
end

function M:onTouchesEnd(e)
	if self.focus == e.info.id then
		self.focus = nil
		self:updateVisualState(false)
		self:dispatchEvent(Event.new("Click"))
		e:stopPropagation()
	end
end

function M:onTouchesCancel(e)
	if self.focus == e.info.id then
		self.focus = nil
		self:updateVisualState(false)
		e:stopPropagation()
	end
end

function M:updateVisualState(state)
	if state then
		if self:contains(self.up) then
			self:removeChild(self.up)
		end
		if not self:contains(self.down) then
			self:addChild(self.down)
		end
	else
		if self:contains(self.down) then
			self:removeChild(self.down)
		end
		if not self:contains(self.up) then
			self:addChild(self.up)
		end
	end
end

return M
