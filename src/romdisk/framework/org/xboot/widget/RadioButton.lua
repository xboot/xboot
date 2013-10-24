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
	self.opt.checked = option.checked or false
	self.opt.imageChecked = assert(option.imageChecked or theme.radiobutton.imageChecked)
	self.opt.imageUnchecked = assert(option.imageUnchecked or theme.radiobutton.imageUnchecked)

	self.touchid = nil
	self.checked = self.opt.checked
	self.frameChecked = DisplayImage.new(asset:loadTexture(self.opt.imageChecked))
	self.frameUnchecked = DisplayImage.new(asset:loadTexture(self.opt.imageUnchecked))

	self:updateVisualState(self.checked)
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
	if self.touchid == nil and self:hitTestPoint(e.info.x, e.info.y) then
		self.touchid = 0
		self.checked = not self.checked
		self:updateVisualState(self.checked)
		self:dispatchEvent(Event.new("Change", {checked = self.checked}))
		e:stopPropagation()
	end
end

function M:onMouseMove(e)
	if self.touchid == 0 then
		if not self:hitTestPoint(e.info.x, e.info.y) then
			self.touchid = nil
		end
		e:stopPropagation()
	end
end

function M:onMouseUp(e)
	if self.touchid == 0 then
		self.touchid = nil
		e:stopPropagation()
	end
end

function M:onTouchesBegin(e)
	if self.touchid == nil and self:hitTestPoint(e.info.x, e.info.y) then
		self.touchid = e.info.id
		self.checked = not self.checked
		self:updateVisualState(self.checked)
		self:dispatchEvent(Event.new("Change", {checked = self.checked}))
		e:stopPropagation()
	end
end

function M:onTouchesMove(e)
	if self.touchid == e.info.id then
		if not self:hitTestPoint(e.info.x, e.info.y) then
			self.touchid = nil
		end
		e:stopPropagation()
	end
end

function M:onTouchesEnd(e)
	if self.touchid == e.info.id then
		self.touchid = nil
		e:stopPropagation()
	end
end

function M:onTouchesCancel(e)
	if self.touchid == e.info.id then
		self.touchid = nil
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
