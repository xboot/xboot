local M = Class(DisplayObject)

M._STATE_NORMAL = "NORMAL"
M._STATE_PRESSED = "PRESSED"

function M:init(width, height, task)
	local w, h = task:getSize()
	local image = task:snapshot():clone(Matrix.new({width / w, 0, 0, height / h, 0, 0}))
	self.super:init(width, height)
	self._task = task
	self._panelNormal = DisplayImage.new(image:clone(0, 0, width, height, 32))
	self._panelPressed = DisplayImage.new(image:clone(0, 0, width, height, 32):sepia())

	self._touchid = nil
	self._state = M._STATE_NORMAL
	self:updateVisualState()

	self:addEventListener("mouse-down", self.onMouseDown)
	self:addEventListener("mouse-move", self.onMouseMove)
	self:addEventListener("mouse-up", self.onMouseUp)
	self:addEventListener("touch-begin", self.onTouchBegin)
	self:addEventListener("touch-move", self.onTouchMove)
	self:addEventListener("touch-end", self.onTouchEnd)
end

function M:execute()
	self._task:toFront()
end

function M:onMouseDown(e)
	if self._state == self._STATE_NORMAL and self:hitTestPoint(e.x, e.y) then
		self._touchid = -1
		self._state = self._STATE_PRESSED
		self:updateVisualState()
		e.stop = true
	end
end

function M:onMouseMove(e)
	if self._state == self._STATE_PRESSED and self._touchid == -1 then
		self._touchid = nil
		self._state = self._STATE_NORMAL
		self:updateVisualState()
		e.stop = true
	end
end

function M:onMouseUp(e)
	if self._state == self._STATE_PRESSED and self._touchid == -1 then
		if self:hitTestPoint(e.x, e.y) then
			self._touchid = nil
			self._state = self._STATE_NORMAL
			self:updateVisualState()
			self:dispatchEvent(Event.new("click"))
			e.stop = true
		end
	end
end

function M:onTouchBegin(e)
	if self._state == self._STATE_NORMAL and self:hitTestPoint(e.x, e.y) then
		self._touchid = e.id
		self._state = self._STATE_PRESSED
		self:updateVisualState()
		e.stop = true
	end
end

function M:onTouchMove(e)
	if self._state == self._STATE_PRESSED and self._touchid == e.id then
		self._touchid = nil
		self._state = self._STATE_NORMAL
		self:updateVisualState()
		e.stop = true
	end
end

function M:onTouchEnd(e)
	if self._state == self._STATE_PRESSED and self._touchid == e.id then
		if self:hitTestPoint(e.x, e.y) then
			self._touchid = nil
			self._state = self._STATE_NORMAL
			self:updateVisualState()
			self:dispatchEvent(Event.new("click"))
			e.stop = true
		end
	end
end

function M:updateVisualState()
	if self._state == self._STATE_NORMAL then
		if self:contains(self._panelPressed) then
			self:removeChild(self._panelPressed)
		end
		if not self:contains(self._panelNormal) then
			self:addChild(self._panelNormal)
		end
	elseif self._state == self._STATE_PRESSED then
		if self:contains(self._panelNormal) then
			self:removeChild(self._panelNormal)
		end
		if not self:contains(self._panelPressed) then
			self:addChild(self._panelPressed)
		end
	end
end

return M
