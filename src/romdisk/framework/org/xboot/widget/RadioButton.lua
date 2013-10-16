local M = Class(DisplayObject)

function M:init(option, name)
	self.super:init()

	local option = option or {}
	local theme = asset:loadTheme(name)

	self.opt = {}
	self.opt.left = option.left or 0
	self.opt.top = option.top or 0
	self.opt.checked = option.checked or false
	self.opt.width = assert(option.width or theme.radiobutton.width)
	self.opt.height = assert(option.height or theme.radiobutton.height)
	self.opt.imageChecked = assert(option.imageChecked or theme.radiobutton.imageChecked)
	self.opt.imageUnchecked = assert(option.imageUnchecked or theme.radiobutton.imageUnchecked)

	self.focus = nil
	self.checked = self.opt.checked
	self.frameChecked = DisplayImage.new(asset:loadTexture(self.opt.imageChecked))
	self.frameUnchecked = DisplayImage.new(asset:loadTexture(self.opt.imageUnchecked))

	self:setPosition(self.opt.left, self.opt.top)
	self:updateVisualState(self.checked)

	self:addEventListener(Event.MOUSE_DOWN, self.onMouseDown, self)
	self:addEventListener(Event.MOUSE_MOVE, self.onMouseMove, self)
	self:addEventListener(Event.MOUSE_UP, self.onMouseUp, self)

	self:addEventListener(Event.TOUCHES_BEGIN, self.onTouchesBegin, self)
	self:addEventListener(Event.TOUCHES_MOVE, self.onTouchesMove, self)
	self:addEventListener(Event.TOUCHES_END, self.onTouchesEnd, self)
	self:addEventListener(Event.TOUCHES_CANCEL, self.onTouchesCancel, self)
end

function M:setChecked(checked)
	if type(checked) == "boolean" and self.checked ~= checked then
		self.checked = checked
		self:updateVisualState(self.checked)
	end
end

function M:getChecked()
	return self.checked
end

function M:onMouseDown(e)
	if self.focus == nil and self:hitTestPoint(e.info.x, e.info.y) then
		self.focus = 0
		self.checked = not self.checked
		self:updateVisualState(self.checked)
		self:dispatchEvent(Event.new("Change", {checked = self.checked}))
		e:stopPropagation()
	end
end

function M:onMouseMove(e)
	if self.focus == 0 then
		if not self:hitTestPoint(e.info.x, e.info.y) then
			self.focus = nil
		end
		e:stopPropagation()
	end
end

function M:onMouseUp(e)
	if self.focus == 0 then
		self.focus = nil
		e:stopPropagation()
	end
end

function M:onTouchesBegin(e)
	if self.focus == nil and self:hitTestPoint(e.info.x, e.info.y) then
		self.focus = e.info.id
		self.checked = not self.checked
		self:updateVisualState(self.checked)
		self:dispatchEvent(Event.new("Change", {checked = self.checked}))
		e:stopPropagation()
	end
end

function M:onTouchesMove(e)
	if self.focus == e.info.id then
		if not self:hitTestPoint(e.info.x, e.info.y) then
			self.focus = nil
		end
		e:stopPropagation()
	end
end

function M:onTouchesEnd(e)
	if self.focus == e.info.id then
		self.focus = nil
		e:stopPropagation()
	end
end

function M:onTouchesCancel(e)
	if self.focus == e.info.id then
		self.focus = nil
		e:stopPropagation()
	end
end

function M:updateVisualState(checked)
	if checked then
		if self:contains(self.frameUnchecked) then
			self:removeChild(self.frameUnchecked)
		end
		if not self:contains(self.frameChecked) then
			self:addChild(self.frameChecked)
		end
	else
		if self:contains(self.frameChecked) then
			self:removeChild(self.frameChecked)
		end
		if not self:contains(self.frameUnchecked) then
			self:addChild(self.frameUnchecked)
		end
	end
end

return M
