local M = Class(DisplayObject)

M._STATE_NORMAL = "NORMAL"
M._STATE_PRESSED = "PRESSED"

function M:init(app)
	local icon = app:getIcon() or assets:loadImage("assets/images/default-icon.png")
	local w, h = icon:getSize()
	self.super:init(w, h)
	self._app = app
	self._panelNormal = DisplayImage.new(icon:clone(0, 0, w, h, 32))
	self._panelPressed = DisplayImage.new(icon:clone(0, 0, w, h, 32):sepia())

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

function M:getPath()
	return self._app:getPath()
end

function M:getName()
	return self._app:getName()
end

function M:getDescription()
	return self._app:getDescription()
end

function M:execute()
	self._app:execute()
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
