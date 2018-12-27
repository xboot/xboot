local Dobject = require "graphic.dobject"

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
	self.opt.imageNormal = assert(option.imageNormal or theme.button.imageNormal)
	self.opt.imagePressed = assert(option.imagePressed or theme.button.imagePressed)
	self.opt.imageDisabled = assert(option.imageDisabled or theme.button.imageDisabled)

	self.frameNormal = assets:loadDisplay(self.opt.imageNormal)
	self.framePressed = assets:loadDisplay(self.opt.imagePressed)
	self.frameDisabled = assets:loadDisplay(self.opt.imageDisabled)

	self.frameNormal:setAlignment(Dobject.ALIGN_CENTER_FILL)
	self.framePressed:setAlignment(Dobject.ALIGN_CENTER_FILL)
	self.frameDisabled:setAlignment(Dobject.ALIGN_CENTER_FILL)

	local width, height = self.frameNormal:getSize()
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
	self.frameNormal:setSize(width, height)
	self.framePressed:setSize(width, height)
	self.frameDisabled:setSize(width, height)
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
		self:dispatchEvent(Event.new("Press"))
		e.stop = true
	end
end

function M:onMouseMove(e)
	if self.state ~= self.STATE_DISABLED and self.touchid == -1 then
		if not self:hitTestPoint(e.x, e.y) then
			self.touchid = nil
			self.state = self.STATE_NORMAL
			self:updateVisualState()
			self:dispatchEvent(Event.new("Release"))
		end
		e.stop = true
	end
end

function M:onMouseUp(e)
	if self.state ~= self.STATE_DISABLED and self.touchid == -1 then
		self.touchid = nil
		self.state = self.STATE_NORMAL
		self:updateVisualState()
		self:dispatchEvent(Event.new("Release"))
		self:dispatchEvent(Event.new("Click"))
		e.stop = true
	end
end

function M:onTouchBegin(e)
	if self.state ~= self.STATE_DISABLED and self:hitTestPoint(e.x, e.y) then
		self.touchid = e.id
		self.state = self.STATE_PRESSED
		self:updateVisualState()
		self:dispatchEvent(Event.new("Press"))
		e.stop = true
	end
end

function M:onTouchMove(e)
	if self.state ~= self.STATE_DISABLED and self.touchid == e.id then
		if not self:hitTestPoint(e.x, e.y) then
			self.touchid = nil
			self.state = self.STATE_NORMAL
			self:updateVisualState()
			self:dispatchEvent(Event.new("Release"))
		end
		e.stop = true
	end
end

function M:onTouchEnd(e)
	if self.state ~= self.STATE_DISABLED and self.touchid == e.id then
		self.touchid = nil
		self.state = self.STATE_NORMAL
		self:updateVisualState()
		self:dispatchEvent(Event.new("Release"))
		self:dispatchEvent(Event.new("Click"))
		e.stop = true
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
	end
	self:layout()
end

return M
