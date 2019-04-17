local M = Class(DisplayObject)

function M:init(list, x, y)
	self.super:init()

	self:setPosition(x or 0, y or 0)
	self.anim = list or {}
	self.frame = 1
	self.nframes = #self.anim
	self.subframe = 0
	
	self:addChild(self.anim[self.frame])
	self:addEventListener("enter-frame", self.onEnterFrame, self)
end

function M:onEnterFrame()
	self.subframe = self.subframe + 1
	
	if self.subframe > 2 then
		self:removeChild(self.anim[self.frame])
		
		self.frame = self.frame + 1
		if self.frame > self.nframes then
			self.frame = 1
		end

		self:addChild(self.anim[self.frame])
		self.subframe = 0
	end
end

return M
