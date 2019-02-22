local Dobject = Dobject

local M = Class(DisplayObject)

M.STATE_NORMAL = "NORMAL"
M.STATE_PRESSED = "PRESSED"
M.STATE_DISABLED = "DISABLED"

function M:init(option, name)
	self.super:init()

	local assets = assets
	local option = option or {}
	local theme = assets:loadTheme(name)

	self.opt = {}
	self.opt.x = option.x or 0
	self.opt.y = option.y or 0
	self.opt.width = option.width
	self.opt.height = option.height
	self.opt.visible = option.visible or true
	self.opt.touchable = option.touchable or true
	self.opt.enable = option.enable or true

	self.opt.imageTrack = assert(option.imageTrack or theme.slider.imageTrack)
	self.opt.imageBackground = assert(option.imageBackground or theme.slider.imageBackground)
	self.opt.imageThumbNormal = assert(option.imageThumbNormal or theme.slider.imageThumbNormal)
	self.opt.imageThumbPressed = assert(option.imageThumbPressed or theme.slider.imageThumbPressed)
	self.opt.imageThumbDisabled = assert(option.imageThumbDisabled or theme.slider.imageThumbDisabled)

	self.frameimageTrack = assets:loadDisplay(self.opt.imageTrack)
	self.frameBackground = assets:loadDisplay(self.opt.imageBackground)
	self.frameThumbNormal = assets:loadDisplay(self.opt.imageThumbNormal)
	self.frameThumbPressed = assets:loadDisplay(self.opt.imageThumbPressed)
	self.frameThumbDisabled = assets:loadDisplay(self.opt.imageThumbDisabled)

	self.frameimageTrack:setAlignment(Dobject.ALIGN_NONE)
	self.frameBackground:setAlignment(Dobject.ALIGN_CENTER_FILL)
	self.frameThumbNormal:setAlignment(Dobject.ALIGN_NONE)
	self.frameThumbPressed:setAlignment(Dobject.ALIGN_NONE)
	self.frameThumbDisabled:setAlignment(Dobject.ALIGN_NONE)

	local width, height = self.frameBackground:getSize()
	self.opt.width = self.opt.width or width
	self.opt.height = self.opt.height or height

	self.touchid = nil
	self.state = M.STATE_NORMAL

	self:setPosition(self.opt.x, self.opt.y)
	self:setSize(self.opt.width, self.opt.height)
	self:setVisible(self.opt.visible)
	self:setTouchable(self.opt.touchable)
	self:setEnable(self.opt.enable)
	self:updateVisualState()

	self:addEventListener(Event.MOUSE_DOWN, self.onMouseDown)
	self:addEventListener(Event.MOUSE_MOVE, self.onMouseMove)
	self:addEventListener(Event.MOUSE_UP, self.onMouseUp)

	self:addEventListener(Event.TOUCH_BEGIN, self.onTouchBegin)
	self:addEventListener(Event.TOUCH_MOVE, self.onTouchMove)
	self:addEventListener(Event.TOUCH_END, self.onTouchEnd)
end

function M:setSize(width, height)
	self.super:setSize(width, height)
	self:updateVisualState()
	return self
end

function M:setEnable(enable)
	if enable then
		self.state = self.STATE_NORMAL
	else
		self.state = self.STATE_DISABLED
	end
	self:updateVisualState()
	return self
end

function M:getEnable()
	if self.state ~= self.STATE_DISABLED then
		return true
	end
	return false
end

function M:enable()
	return self:setEnable(true)
end

function M:disable()
	return self:setEnable(false)
end

function M:onMouseDown(e)
	if self.state ~= self.STATE_DISABLED and self:hitTestPoint(e.x, e.y) then
		self.touchid = -1
		self.state = self.STATE_PRESSED
		self:updateVisualState()
		e.stop = true
	end
end

function M:onMouseMove(e)
	if self.state ~= self.STATE_DISABLED and self.touchid == -1 then
		if not self:hitTestPoint(e.x, e.y) then
			self.touchid = nil
			self.state = self.STATE_NORMAL
			self:updateVisualState()
		end
		e.stop = true
	end
end

function M:onMouseUp(e)
	if self.state ~= self.STATE_DISABLED and self.touchid == -1 then
		self.touchid = nil
		self.state = self.STATE_NORMAL
		self.checked = not self.checked
		self:updateVisualState()
		self:dispatchEvent(Event.new("Change", {checked = self.checked}))
		e.stop = true
	end
end

function M:onTouchBegin(e)
	if self.state ~= self.STATE_DISABLED and self:hitTestPoint(e.x, e.y) then
		self.touchid = e.id
		self.state = self.STATE_PRESSED
		self:updateVisualState()
		e.stop = true
	end
end

function M:onTouchMove(e)
	if self.state ~= self.STATE_DISABLED and self.touchid == e.id then
		if not self:hitTestPoint(e.x, e.y) then
			self.touchid = nil
			self.state = self.STATE_NORMAL
			self:updateVisualState()
		end
		e.stop = true
	end
end

function M:onTouchEnd(e)
	if self.state ~= self.STATE_DISABLED and self.touchid == e.id then
		self.touchid = nil
		self.state = self.STATE_NORMAL
		self.checked = not self.checked
		self:updateVisualState()
		self:dispatchEvent(Event.new("Change", {checked = self.checked}))
		e.stop = true
	end
end

function M:updateVisualState()
	if self.checked then
		if self:contains(self.frameOffNormal) then
			self:removeChild(self.frameOffNormal)
		end
		if self:contains(self.frameOffPressed) then
			self:removeChild(self.frameOffPressed)
		end
		if self:contains(self.frameOffDisabled) then
			self:removeChild(self.frameOffDisabled)
		end

		if self.state == self.STATE_NORMAL then
			if self:contains(self.frameOnPressed) then
				self:removeChild(self.frameOnPressed)
			end
			if self:contains(self.frameOnDisabled) then
				self:removeChild(self.frameOnDisabled)
			end
			if not self:contains(self.frameOnNormal) then
				self:addChild(self.frameOnNormal)
			end
		elseif self.state == self.STATE_PRESSED then
			if self:contains(self.frameOnNormal) then
				self:removeChild(self.frameOnNormal)
			end
			if self:contains(self.frameOnDisabled) then
				self:removeChild(self.frameOnDisabled)
			end
			if not self:contains(self.frameOnPressed) then
				self:addChild(self.frameOnPressed)
			end
		elseif self.state == self.STATE_DISABLED then
			if self:contains(self.frameOnNormal) then
				self:removeChild(self.frameOnNormal)
			end
			if self:contains(self.frameOnPressed) then
				self:removeChild(self.frameOnPressed)
			end
			if not self:contains(self.frameOnDisabled) then
				self:addChild(self.frameOnDisabled)
			end
		end
	else
		if self:contains(self.frameOnNormal) then
			self:removeChild(self.frameOnNormal)
		end
		if self:contains(self.frameOnPressed) then
			self:removeChild(self.frameOnPressed)
		end
		if self:contains(self.frameOnDisabled) then
			self:removeChild(self.frameOnDisabled)
		end

		if self.state == self.STATE_NORMAL then
			if self:contains(self.frameOffPressed) then
				self:removeChild(self.frameOffPressed)
			end
			if self:contains(self.frameOffDisabled) then
				self:removeChild(self.frameOffDisabled)
			end
			if not self:contains(self.frameOffNormal) then
				self:addChild(self.frameOffNormal)
			end
		elseif self.state == self.STATE_PRESSED then
			if self:contains(self.frameOffNormal) then
				self:removeChild(self.frameOffNormal)
			end
			if self:contains(self.frameOffDisabled) then
				self:removeChild(self.frameOffDisabled)
			end
			if not self:contains(self.frameOffPressed) then
				self:addChild(self.frameOffPressed)
			end
		elseif self.state == self.STATE_DISABLED then
			if self:contains(self.frameOffNormal) then
				self:removeChild(self.frameOffNormal)
			end
			if self:contains(self.frameOffPressed) then
				self:removeChild(self.frameOffPressed)
			end
			if not self:contains(self.frameOffDisabled) then
				self:addChild(self.frameOffDisabled)
			end
		end
	end
	self:layout()
end

return M
