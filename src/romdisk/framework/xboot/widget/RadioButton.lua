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
	self.opt.checked = option.checked or false

	self.opt.imageOnNormal = assert(option.imageOnNormal or theme.radiobutton.imageOnNormal)
	self.opt.imageOnPressed = assert(option.imageOnPressed or theme.radiobutton.imageOnPressed)
	self.opt.imageOnDisabled = assert(option.imageOnDisabled or theme.radiobutton.imageOnDisabled)
	self.opt.imageOffNormal = assert(option.imageOffNormal or theme.radiobutton.imageOffNormal)
	self.opt.imageOffPressed = assert(option.imageOffPressed or theme.radiobutton.imageOffPressed)
	self.opt.imageOffDisabled = assert(option.imageOffDisabled or theme.radiobutton.imageOffDisabled)

	self.frameOnNormal = assets:loadDisplay(self.opt.imageOnNormal)
	self.frameOnPressed = assets:loadDisplay(self.opt.imageOnPressed)
	self.frameOnDisabled = assets:loadDisplay(self.opt.imageOnDisabled)
	self.frameOffNormal = assets:loadDisplay(self.opt.imageOffNormal)
	self.frameOffPressed = assets:loadDisplay(self.opt.imageOffPressed)
	self.frameOffDisabled = assets:loadDisplay(self.opt.imageOffDisabled)

	self.frameOnNormal:setAlignment(Dobject.ALIGN_CENTER_FILL)
	self.frameOnPressed:setAlignment(Dobject.ALIGN_CENTER_FILL)
	self.frameOnDisabled:setAlignment(Dobject.ALIGN_CENTER_FILL)
	self.frameOffNormal:setAlignment(Dobject.ALIGN_CENTER_FILL)
	self.frameOffPressed:setAlignment(Dobject.ALIGN_CENTER_FILL)
	self.frameOffDisabled:setAlignment(Dobject.ALIGN_CENTER_FILL)

	local width, height = self.frameOnNormal:getSize()
	self.opt.width = self.opt.width or width
	self.opt.height = self.opt.height or height

	self.touchid = nil
	self.state = M.STATE_NORMAL
	self.checked = self.opt.checked

	self:setPosition(self.opt.x, self.opt.y)
	self:setSize(self.opt.width, self.opt.height)
	self:setVisible(self.opt.visible)
	self:setTouchable(self.opt.touchable)
	self:setEnable(self.opt.enable)
	self:setChecked(self.opt.checked)
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

function M:setChecked(checked)
	self.checked = checked
	self:updateVisualState()
	return self
end

function M:getChecked()
	return self.checked
end

function M:enable()
	return self:setEnable(true)
end

function M:disable()
	return self:setEnable(false)
end

function M:checked()
	return self:setChecked(true)
end

function M:unchecked()
	return self:setChecked(false)
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
		self.checked = true
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
		self.checked = true
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
