---
-- @module Button
local M = Class(DisplayObject)

---
-- @field [parent=#Button] Button state
M.STATE_NORMAL = "NORMAL"
M.STATE_PRESSED = "PRESSED"
M.STATE_DISABLED = "DISABLED"

---
-- @field [parent=#Button] Button event type
M.EVENT_PRESS	= "Press"
M.EVENT_RELEASE = "Release"
M.EVENT_CLICK	= "Click"

function M:init(option, name)
	self.super:init()

	local asset = application:getAsset()
	local option = option or {}
	local theme = asset:loadTheme(name)

	self.opt = {}
	self.opt.x = option.x or 0
	self.opt.y = option.y or 0
	self.opt.width = option.width
	self.opt.height = option.height
	self.opt.imageNormal = assert(option.imageNormal or theme.button.imageNormal)
	self.opt.imagePressed = assert(option.imagePressed or theme.button.imagePressed)
	self.opt.imageDisabled = assert(option.imagePressed or theme.button.imageDisabled)

	self.touchid = nil
	self.state = M.STATE_NORMAL
	self.frameNormal = asset:loadDisplay(self.opt.imageNormal)
	self.framePressed = asset:loadDisplay(self.opt.imagePressed)
	self.frameDisabled = asset:loadDisplay(self.opt.imageDisabled)

	self:updateVisualState()
	self:setPosition(self.opt.x, self.opt.y)
	self:setContentSize(self.opt.width, self.opt.height)

	self:addEventListener(Event.MOUSE_DOWN, self.onMouseDown, self)
	self:addEventListener(Event.MOUSE_MOVE, self.onMouseMove, self)
	self:addEventListener(Event.MOUSE_UP, self.onMouseUp, self)

	self:addEventListener(Event.TOUCHES_BEGIN, self.onTouchesBegin, self)
	self:addEventListener(Event.TOUCHES_MOVE, self.onTouchesMove, self)
	self:addEventListener(Event.TOUCHES_END, self.onTouchesEnd, self)
	self:addEventListener(Event.TOUCHES_CANCEL, self.onTouchesCancel, self)
end

function M:setContentSize(width, height)
	self.frameNormal:setContentSize(width, height)
	self.framePressed:setContentSize(width, height)
	self.frameDisabled:setContentSize(width, height)
	return self
end

function M:setEnable(enable)
	if enable then
		if self.state == self.STATE_DISABLED then
			self:setTouchable(true)
			self.state = self.STATE_NORMAL
			self:updateVisualState()
		end
	else
		if self.state ~= self.STATE_DISABLED then
			self:setTouchable(false)
			self.state = self.STATE_DISABLED
			self:updateVisualState()
		end	
	end
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
	if self.touchid == nil and self:hitTestPoint(e.info.x, e.info.y) then
		self.touchid = 0
		self.state = self.STATE_PRESSED
		self:updateVisualState()
		self:dispatchEvent(Event.new(self.EVENT_PRESS))
		e:stopPropagation()
	end
end

function M:onMouseMove(e)
	if self.touchid == 0 then
		if not self:hitTestPoint(e.info.x, e.info.y) then
			self.touchid = nil
			self.state = self.STATE_NORMAL
			self:updateVisualState()
			self:dispatchEvent(Event.new(self.EVENT_RELEASE))
		end
		e:stopPropagation()
	end
end

function M:onMouseUp(e)
	if self.touchid == 0 then
		self.touchid = nil
		self.state = self.STATE_NORMAL
		self:updateVisualState()
		self:dispatchEvent(Event.new(self.EVENT_RELEASE))
		self:dispatchEvent(Event.new(self.EVENT_CLICK))
		e:stopPropagation()
	end
end

function M:onTouchesBegin(e)
	if self.touchid == nil and self:hitTestPoint(e.info.x, e.info.y) then
		self.touchid = e.info.id
		self.state = self.STATE_PRESSED
		self:updateVisualState()
		self:dispatchEvent(Event.new(self.EVENT_PRESS))
		e:stopPropagation()
	end
end

function M:onTouchesMove(e)
	if self.touchid == e.info.id then
		if not self:hitTestPoint(e.info.x, e.info.y) then
			self.touchid = nil
			self.state = self.STATE_NORMAL
			self:updateVisualState()
			self:dispatchEvent(Event.new(self.EVENT_RELEASE))
		end
		e:stopPropagation()
	end
end

function M:onTouchesEnd(e)
	if self.touchid == e.info.id then
		self.touchid = nil
		self.state = self.STATE_NORMAL
		self:updateVisualState()
		self:dispatchEvent(Event.new(self.EVENT_RELEASE))
		self:dispatchEvent(Event.new(self.EVENT_CLICK))
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
end

return M
