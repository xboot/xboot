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
		self.on:setVisible(true)
		self.off:setVisible(false)
	else
		self.on:setVisible(false)
		self.off:setVisible(true)
	end	
end

return M
