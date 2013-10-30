local M = Class(DisplayObject)

function M:init(option, name)
	self.super:init()

	local option = option or {}
	local theme = asset:loadTheme(name)

	self.opt = {}
	self.opt.x = option.x or 0
	self.opt.y = option.y or 0
	self.opt.width = option.width
	self.opt.height = option.height
	self.opt.imageUp = assert(option.imageUp or theme.button.imageUp)
	self.opt.imageDown = assert(option.imageDown or theme.button.imageDown)

	self.touchid = nil
	self.pressed = false
	self.frameUp = DisplayImage.new(asset:loadTexture(self.opt.imageUp))
	self.frameDown = DisplayImage.new(asset:loadTexture(self.opt.imageDown))

	self:updateVisualState(self.pressed)
	self:setPosition(self.opt.x, self.opt.y)
	self:fitSize(self.opt.width, self.opt.height)

	self:addEventListener(Event.MOUSE_DOWN, self.onMouseDown, self)
	self:addEventListener(Event.MOUSE_MOVE, self.onMouseMove, self)
	self:addEventListener(Event.MOUSE_UP, self.onMouseUp, self)

	self:addEventListener(Event.TOUCHES_BEGIN, self.onTouchesBegin, self)
	self:addEventListener(Event.TOUCHES_MOVE, self.onTouchesMove, self)
	self:addEventListener(Event.TOUCHES_END, self.onTouchesEnd, self)
	self:addEventListener(Event.TOUCHES_CANCEL, self.onTouchesCancel, self)
end

function M:getPressed()
	return self.pressed
end

function M:onMouseDown(e)
	if self.touchid == nil and self:hitTestPoint(e.info.x, e.info.y) then
		self.touchid = 0
		self.pressed = true
		self:updateVisualState(self.pressed)
		e:stopPropagation()
	end
end

function M:onMouseMove(e)
	if self.touchid == 0 then
		if not self:hitTestPoint(e.info.x, e.info.y) then
			self.touchid = nil
			self.pressed = false
			self:updateVisualState(self.pressed)
		end
		e:stopPropagation()
	end
end

function M:onMouseUp(e)
	if self.touchid == 0 then
		self.touchid = nil
		self.pressed = false
		self:updateVisualState(self.pressed)
		self:dispatchEvent(Event.new("Click"))
		e:stopPropagation()
	end
end

function M:onTouchesBegin(e)
	if self.touchid == nil and self:hitTestPoint(e.info.x, e.info.y) then
		self.touchid = e.info.id
		self.pressed = true
		self:updateVisualState(self.pressed)
		e:stopPropagation()
	end
end

function M:onTouchesMove(e)
	if self.touchid == e.info.id then
		if not self:hitTestPoint(e.info.x, e.info.y) then
			self.touchid = nil
			self.pressed = false
			self:updateVisualState(self.pressed)
		end
		e:stopPropagation()
	end
end

function M:onTouchesEnd(e)
	if self.touchid == e.info.id then
		self.touchid = nil
		self.pressed = false
		self:updateVisualState(self.pressed)
		self:dispatchEvent(Event.new("Click"))
		e:stopPropagation()
	end
end

function M:onTouchesCancel(e)
	if self.touchid == e.info.id then
		self.touchid = nil
		self.pressed = false
		self:updateVisualState(self.pressed)
		e:stopPropagation()
	end
end

function M:updateVisualState(pressed)
	if pressed then
		if self:contains(self.frameUp) then
			self:removeChild(self.frameUp)
		end
		if not self:contains(self.frameDown) then
			self:addChild(self.frameDown)
		end
	else
		if self:contains(self.frameDown) then
			self:removeChild(self.frameDown)
		end
		if not self:contains(self.frameUp) then
			self:addChild(self.frameUp)
		end
	end
end

return M
