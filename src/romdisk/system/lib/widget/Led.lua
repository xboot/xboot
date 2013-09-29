local M = Class(DisplayObject)

function M:init(on, off, state)
	self.super:init()

	self.on = assert(on)
	self.off = assert(off)
	self.state = type(state) == "boolean" and state or false
	self:updateVisualState(self.state)
end

function M:setState(state)
	if type(state) == "boolean" and self.state ~= state then
		self.state = state
		self:updateVisualState(self.state)
	end
end

function M:getState()
	return self.state
end

function M:updateVisualState(state)
	if state then
		if self:contains(self.off) then
			self:removeChild(self.off)
		end
		if not self:contains(self.on) then
			self:addChild(self.on)
		end
	else
		if self:contains(self.on) then
			self:removeChild(self.on)
		end
		if not self:contains(self.off) then
			self:addChild(self.off)
		end
	end
end

return M
