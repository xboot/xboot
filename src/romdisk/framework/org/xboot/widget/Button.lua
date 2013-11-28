---
-- @module Button
local M = Class(DisplayObject)

M.STATE_NORMAL = "NORMAL"
M.STATE_PRESSED = "PRESSED"
M.STATE_DISABLED = "DISABLED"

function M:init(option, name)
	self.super:init()

	local assets = application:getAssets()
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
	self.opt.imageDisabled = assert(option.imagePressed or theme.button.imageDisabled)

	self.frameNormal = assets:loadDisplay(self.opt.imageNormal)
	self.framePressed = assets:loadDisplay(self.opt.imagePressed)
	self.frameDisabled = assets:loadDisplay(self.opt.imageDisabled)

	self.frameNormal:setAlignment(self.ALIGN_CENTER_FILL)
	self.framePressed:setAlignment(self.ALIGN_CENTER_FILL)
	self.frameDisabled:setAlignment(self.ALIGN_CENTER_FILL)

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

	self:addEventListener(Event.MOUSE_DOWN, self.onMouseDown, self)
	self:addEventListener(Event.MOUSE_MOVE, self.onMouseMove, self)
	self:addEventListener(Event.MOUSE_UP, self.onMouseUp, self)

	self:addEventListener(Event.TOUCHES_BEGIN, self.onTouchesBegin, self)
	self:addEventListener(Event.TOUCHES_MOVE, self.onTouchesMove, self)
	self:addEventListener(Event.TOUCHES_END, self.onTouchesEnd, self)
	self:addEventListener(Event.TOUCHES_CANCEL, self.onTouchesCancel, self)
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
	if self.touchid == nil and self.state ~= self.STATE_DISABLED and self:hitTestPoint(e.info.x, e.info.y) then
		self.touchid = 0
		self.state = self.STATE_PRESSED
		self:updateVisualState()
		self:dispatchEvent(Event.new("Press"))
		e:stopPropagation()
	end
end

function M:onMouseMove(e)
	if self.touchid == 0 then
		if not self:hitTestPoint(e.info.x, e.info.y) then
			self.touchid = nil
			self.state = self.STATE_NORMAL
			self:updateVisualState()
			self:dispatchEvent(Event.new("Release"))
		end
		e:stopPropagation()
	end
end

function M:onMouseUp(e)
	if self.touchid == 0 then
		self.touchid = nil
		self.state = self.STATE_NORMAL
		self:updateVisualState()
		self:dispatchEvent(Event.new("Release"))
		self:dispatchEvent(Event.new("Click"))
		e:stopPropagation()
	end
end

function M:onTouchesBegin(e)
	if self.touchid == nil and self.state ~= self.STATE_DISABLED and self:hitTestPoint(e.info.x, e.info.y) then
		self.touchid = e.info.id
		self.state = self.STATE_PRESSED
		self:updateVisualState()
		self:dispatchEvent(Event.new("Press"))
		e:stopPropagation()
	end
end

function M:onTouchesMove(e)
	if self.touchid == e.info.id then
		if not self:hitTestPoint(e.info.x, e.info.y) then
			self.touchid = nil
			self.state = self.STATE_NORMAL
			self:updateVisualState()
			self:dispatchEvent(Event.new("Release"))
		end
		e:stopPropagation()
	end
end

function M:onTouchesEnd(e)
	if self.touchid == e.info.id then
		self.touchid = nil
		self.state = self.STATE_NORMAL
		self:updateVisualState()
		self:dispatchEvent(Event.new("Release"))
		self:dispatchEvent(Event.new("Click"))
		e:stopPropagation()
	end
end

function M:onTouchesCancel(e)
	if self.touchid == e.info.id then
		self.touchid = nil
		self.state = self.STATE_NORMAL
		self:updateVisualState()
		e:stopPropagation()
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
