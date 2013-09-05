local M = Class(DisplayObject)

function M:init(on, off, state)
	DisplayObject.init(self)

	self.on = on
	self.off = off
	self.state = state or false

	self:addChild(self.on)
	self:addChild(self.off)
	self:setState(self.state)
end

function M:setState(state)
	if state then
		self.on:visible(true)
		self.off:visible(false)
		self.width = self.off.width
		self.height = self.off.height
	else
		self.on:visible(false)
		self.off:visible(true)
		self.width = self.on.width
		self.height = self.on.height
	end	
end

return M
