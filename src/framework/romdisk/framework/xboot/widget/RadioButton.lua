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
	self.opt.visible = option.visible or true
	self.opt.touchable = option.touchable or true
	self.opt.enable = option.enable or true
	self.opt.checked = option.checked or false
	self.opt.text = option.text
	self.opt.imageOnNormal = assert(option.imageOnNormal or theme.radiobutton.image.onNormal)
	self.opt.imageOnPressed = assert(option.imageOnPressed or theme.radiobutton.image.onPressed)
	self.opt.imageOnDisabled = assert(option.imageOnDisabled or theme.radiobutton.image.onDisabled)
	self.opt.imageOffNormal = assert(option.imageOffNormal or theme.radiobutton.image.offNormal)
	self.opt.imageOffPressed = assert(option.imageOffPressed or theme.radiobutton.image.offPressed)
	self.opt.imageOffDisabled = assert(option.imageOffDisabled or theme.radiobutton.image.offDisabled)
	self.opt.textFontFamily = assert(option.textFontFamily or theme.radiobutton.text.fontFamily)
	self.opt.textFontSize = assert(option.textFontSize or theme.radiobutton.text.fontSize)
	self.opt.textColorNormal = assert(option.textColorNormal or theme.radiobutton.text.color.normal)
	self.opt.textColorPressed = assert(option.textColorPressed or theme.radiobutton.text.color.pressed)
	self.opt.textColorDisabled = assert(option.textColorDisabled or theme.radiobutton.text.color.disabled)
	self.opt.textMarginLeft = assert(option.textMarginLeft or theme.radiobutton.text.margin.left)
	self.opt.textMarginTop = assert(option.textMarginTop or theme.radiobutton.text.margin.top)
	self.opt.textMarginRight = assert(option.textMarginRight or theme.radiobutton.text.margin.right)
	self.opt.textMarginBottom = assert(option.textMarginBottom or theme.radiobutton.text.margin.bottom)

	self.frameOnNormal = assets:loadDisplay(self.opt.imageOnNormal):setLayoutEnable(true)
	self.frameOnPressed = assets:loadDisplay(self.opt.imageOnPressed):setLayoutEnable(true)
	self.frameOnDisabled = assets:loadDisplay(self.opt.imageOnDisabled):setLayoutEnable(true)
	self.frameOffNormal = assets:loadDisplay(self.opt.imageOffNormal):setLayoutEnable(true)
	self.frameOffPressed = assets:loadDisplay(self.opt.imageOffPressed):setLayoutEnable(true)
	self.frameOffDisabled = assets:loadDisplay(self.opt.imageOffDisabled):setLayoutEnable(true)

	local width, height = self.frameOnNormal:getSize()
	self.opt.width = width
	self.opt.height = height

	self.touchid = nil
	self.state = M.STATE_NORMAL
	self.checked = self.opt.checked

	self:setPosition(self.opt.x, self.opt.y)
	self:setVisible(self.opt.visible)
	self:setTouchable(self.opt.touchable)
	self:setEnable(self.opt.enable)
	self:setChecked(self.opt.checked)
	self:setText(self.opt.text)
	self:setSize(0, 0)
	self:setLayoutDirection("row"):setLayoutJustify("start"):setLayoutAlign("center")
	self:updateVisualState()

	self:addEventListener("mouse-down", self.onMouseDown)
	self:addEventListener("mouse-move", self.onMouseMove)
	self:addEventListener("mouse-up", self.onMouseUp)

	self:addEventListener("touch-begin", self.onTouchBegin)
	self:addEventListener("touch-move", self.onTouchMove)
	self:addEventListener("touch-end", self.onTouchEnd)
end

function M:getAutoSize()
	local width, height = 0, 0
	local left, top, right, bottom = 0, 0, 0, 0
	if self.text then
		width, height = self.text:getSize()
		left, top, right, bottom = self.text:getMargin()
	end
	return self.opt.width + width + left + right, math.max(self.opt.height, height + top + bottom)
end

function M:setWidth(width)
	local w, h = self:getAutoSize()
	self.super:setWidth(w)
	self:updateVisualState()
	return self
end

function M:setHeight(height)
	local w, h = self:getAutoSize()
	self.super:setHeight(h)
	self:updateVisualState()
	return self
end

function M:setSize(width, height)
	local w, h = self:getAutoSize()
	self.super:setSize(w, h)
	self:updateVisualState()
	return self
end

function M:setText(text)
	if text then
		if self.text then
			self.text:setText(text)
		else
			self.text = DisplayText.new(text, self.opt.textColorNormal, self.opt.textFontFamily, self.opt.textFontSize)
			self.text:setLayoutMargin(self.opt.textMarginLeft, self.opt.textMarginTop, self.opt.textMarginRight, self.opt.textMarginBottom)
			self.text:setLayoutEnable(true)
		end
	else
		self.text = nil
	end
	local w, h = self:getAutoSize()
	self.super:setSize(w, h)
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
	if self.state == self.STATE_DISABLED then
		return false
	end
	return true
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
	if self.state == self.STATE_NORMAL and self:hitTestPoint(e.x, e.y) then
		self.touchid = -1
		self.state = self.STATE_PRESSED
		self:updateVisualState()
		e.stop = true
	end
end

function M:onMouseMove(e)
	if self.state == self.STATE_PRESSED and self.touchid == -1 then
		if not self:hitTestPoint(e.x, e.y) then
			self.touchid = nil
			self.state = self.STATE_NORMAL
			self:updateVisualState()
		end
		e.stop = true
	end
end

function M:onMouseUp(e)
	if self.state == self.STATE_PRESSED and self.touchid == -1 then
		if self:hitTestPoint(e.x, e.y) then
			self.touchid = nil
			self.state = self.STATE_NORMAL
			self.checked = true
			self:updateVisualState()
			self:dispatchEvent(Event.new("change", {checked = self.checked}))
			e.stop = true
		end
	end
end

function M:onTouchBegin(e)
	if self.state == self.STATE_NORMAL and self:hitTestPoint(e.x, e.y) then
		self.touchid = e.id
		self.state = self.STATE_PRESSED
		self:updateVisualState()
		e.stop = true
	end
end

function M:onTouchMove(e)
	if self.state == self.STATE_PRESSED and self.touchid == e.id then
		if not self:hitTestPoint(e.x, e.y) then
			self.touchid = nil
			self.state = self.STATE_NORMAL
			self:updateVisualState()
		end
		e.stop = true
	end
end

function M:onTouchEnd(e)
	if self.state == self.STATE_PRESSED and self.touchid == e.id then
		if self:hitTestPoint(e.x, e.y) then
			self.touchid = nil
			self.state = self.STATE_NORMAL
			self.checked = true
			self:updateVisualState()
			self:dispatchEvent(Event.new("change", {checked = self.checked}))
			e.stop = true
		end
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
			if self.text then
				if not self:contains(self.text) then
					self:addChild(self.text)
				end
				self.text:toFront():setColor(self.opt.textColorNormal)
			else
				if self:contains(self.text) then
					self:removeChild(self.text)
				end
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
			if self.text then
				if not self:contains(self.text) then
					self:addChild(self.text)
				end
				self.text:toFront():setColor(self.opt.textColorPressed)
			else
				if self:contains(self.text) then
					self:removeChild(self.text)
				end
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
			if self.text then
				if not self:contains(self.text) then
					self:addChild(self.text)
				end
				self.text:toFront():setColor(self.opt.textColorDisabled)
			else
				if self:contains(self.text) then
					self:removeChild(self.text)
				end
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
			if self.text then
				if not self:contains(self.text) then
					self:addChild(self.text)
				end
				self.text:toFront():setColor(self.opt.textColorNormal)
			else
				if self:contains(self.text) then
					self:removeChild(self.text)
				end
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
			if self.text then
				if not self:contains(self.text) then
					self:addChild(self.text)
				end
				self.text:toFront():setColor(self.opt.textColorPressed)
			else
				if self:contains(self.text) then
					self:removeChild(self.text)
				end
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
			if self.text then
				if not self:contains(self.text) then
					self:addChild(self.text)
				end
				self.text:toFront():setColor(self.opt.textColorDisabled)
			else
				if self:contains(self.text) then
					self:removeChild(self.text)
				end
			end
		end
	end
end

return M
