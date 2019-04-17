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

	self:addEventListener("mouse-down", self.onMouseDown, self)
	self:addEventListener("mouse-move", self.onMouseMove, self)
	self:addEventListener("mouse-up", self.onMouseUp, self)

	self:addEventListener("touch-begin", self.onTouchBegin, self)
	self:addEventListener("touch-move", self.onTouchMove, self)
	self:addEventListener("touch-end", self.onTouchEnd, self)
end

function M:onMouseDown(e)
	if self.focus == nil and self:hitTestPoint(e.info.x, e.info.y) then
		self.focus = 0
		self.ison = not self.ison
		self:updateVisualState(self.ison)
		self:dispatchEvent(Event.new("toggled", {on = self.ison}))
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
		self:dispatchEvent(Event.new("toggled", {on = self.ison}))
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
