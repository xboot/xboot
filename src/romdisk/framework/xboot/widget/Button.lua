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
	self.opt.text = option.text
	self.opt.imageNormal = assert(option.imageNormal or theme.button.image.normal)
	self.opt.imagePressed = assert(option.imagePressed or theme.button.image.pressed)
	self.opt.imageDisabled = assert(option.imageDisabled or theme.button.image.disabled)
	self.opt.textFontFamily = assert(option.textFontFamily or theme.button.text.fontFamily)
	self.opt.textFontSize = assert(option.textFontSize or theme.button.text.fontSize)
	self.opt.textColorNormal = assert(option.textColorNormal or theme.button.text.color.normal)
	self.opt.textColorPressed = assert(option.textColorPressed or theme.button.text.color.pressed)
	self.opt.textColorDisabled = assert(option.textColorDisabled or theme.button.text.color.disabled)
	self.opt.textMarginLeft = assert(option.textMarginLeft or theme.button.text.margin.left)
	self.opt.textMarginTop = assert(option.textMarginTop or theme.button.text.margin.top)
	self.opt.textMarginRight = assert(option.textMarginRight or theme.button.text.margin.right)
	self.opt.textMarginBottom = assert(option.textMarginBottom or theme.button.text.margin.bottom)

	self.frameNormal = assets:loadDisplay(self.opt.imageNormal):setLayoutSpecial(true):setLayoutEnable(true)
	self.framePressed = assets:loadDisplay(self.opt.imagePressed):setLayoutSpecial(true):setLayoutEnable(true)
	self.frameDisabled = assets:loadDisplay(self.opt.imageDisabled):setLayoutSpecial(true):setLayoutEnable(true)

	local width, height = self.frameNormal:getSize()
	self.opt.width = self.opt.width or width
	self.opt.height = self.opt.height or height

	self.touchid = nil
	self.state = M.STATE_NORMAL

	self:setLayoutDirection("row"):setLayoutJustify("center"):setLayoutAlign("center")
	self:setPosition(self.opt.x, self.opt.y)
	self:setSize(self.opt.width, self.opt.height)
	self:setVisible(self.opt.visible)
	self:setTouchable(self.opt.touchable)
	self:setEnable(self.opt.enable)
	self:setText(self.opt.text)
	self:updateVisualState()

	self:addEventListener("mouse-down", self.onMouseDown)
	self:addEventListener("mouse-move", self.onMouseMove)
	self:addEventListener("mouse-up", self.onMouseUp)

	self:addEventListener("touch-begin", self.onTouchBegin)
	self:addEventListener("touch-move", self.onTouchMove)
	self:addEventListener("touch-end", self.onTouchEnd)
end

function M:setWidth(width)
	self.super:setWidth(width)
	self.frameNormal:setWidth(width)
	self.framePressed:setWidth(width)
	self.frameDisabled:setWidth(width)
	return self
end

function M:setHeight(height)
	self.super:setHeight(height)
	self.frameNormal:setHeight(height)
	self.framePressed:setHeight(height)
	self.frameDisabled:setHeight(height)
	return self
end

function M:setSize(width, height)
	self.super:setSize(width, height)
	self.frameNormal:setSize(width, height)
	self.framePressed:setSize(width, height)
	self.frameDisabled:setSize(width, height)
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

function M:enable()
	return self:setEnable(true)
end

function M:disable()
	return self:setEnable(false)
end

function M:onMouseDown(e)
	if self.state == self.STATE_NORMAL and self:hitTestPoint(e.x, e.y) then
		self.touchid = -1
		self.state = self.STATE_PRESSED
		self:updateVisualState()
		self:dispatchEvent(Event.new("press"))
		e.stop = true
	end
end

function M:onMouseMove(e)
	if self.state == self.STATE_PRESSED and self.touchid == -1 then
		if not self:hitTestPoint(e.x, e.y) then
			self.touchid = nil
			self.state = self.STATE_NORMAL
			self:updateVisualState()
			self:dispatchEvent(Event.new("release"))
		end
		e.stop = true
	end
end

function M:onMouseUp(e)
	if self.state == self.STATE_PRESSED and self.touchid == -1 then
		if self:hitTestPoint(e.x, e.y) then
			self.touchid = nil
			self.state = self.STATE_NORMAL
			self:updateVisualState()
			self:dispatchEvent(Event.new("release"))
			self:dispatchEvent(Event.new("click"))
			e.stop = true
		end
	end
end

function M:onTouchBegin(e)
	if self.state == self.STATE_NORMAL and self:hitTestPoint(e.x, e.y) then
		self.touchid = e.id
		self.state = self.STATE_PRESSED
		self:updateVisualState()
		self:dispatchEvent(Event.new("press"))
		e.stop = true
	end
end

function M:onTouchMove(e)
	if self.state == self.STATE_PRESSED and self.touchid == e.id then
		if not self:hitTestPoint(e.x, e.y) then
			self.touchid = nil
			self.state = self.STATE_NORMAL
			self:updateVisualState()
			self:dispatchEvent(Event.new("release"))
		end
		e.stop = true
	end
end

function M:onTouchEnd(e)
	if self.state == self.STATE_PRESSED and self.touchid == e.id then
		if self:hitTestPoint(e.x, e.y) then
			self.touchid = nil
			self.state = self.STATE_NORMAL
			self:updateVisualState()
			self:dispatchEvent(Event.new("release"))
			self:dispatchEvent(Event.new("click"))
			e.stop = true
		end
	end
end

function M:updateVisualState()
	if self.state == self.STATE_NORMAL then
		if self:contains(self.framePressed) then
			self:removeChild(self.framePressed)
		end
		if self:contains(self.frameDisabled) then
			self:removeChild(self.frameDisabled)
		end
		if not self:contains(self.frameNormal) then
			self:addChild(self.frameNormal)
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
		if self:contains(self.frameNormal) then
			self:removeChild(self.frameNormal)
		end
		if self:contains(self.frameDisabled) then
			self:removeChild(self.frameDisabled)
		end
		if not self:contains(self.framePressed) then
			self:addChild(self.framePressed)
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
		if self:contains(self.frameNormal) then
			self:removeChild(self.frameNormal)
		end
		if self:contains(self.framePressed) then
			self:removeChild(self.framePressed)
		end
		if not self:contains(self.frameDisabled) then
			self:addChild(self.frameDisabled)
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

return M
