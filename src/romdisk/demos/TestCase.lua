local M = Class(DisplayObject)

function M:init(cases, w, h)
	self.super:init()

	local width, height = application:getScreenSize()
	self.w = w or width
	self.h = h or height
	self.cases = cases or {}
	if #self.cases > 0 then
		self.index = 1
		self.case = self.cases[self.index].new()
		self:addChild(self.case)
--		self.oldcase = self.case
--		self.newcase = self.case
	end
end

function M:prev()
	self.index = self.index - 1
	if self.index < 1 then
		self.index = 1
		self.case:setX(self.w / 8):animate({x = 0}, 0.5, "outBounce")
		return
	end
	
	local oldcase = self.case
	local newcase = self.cases[self.index].new()
	newcase:setPosition(-self.w, 0)
	newcase:animate({x = 0}, 1, "outBounce")
	newcase:addEventListener(Event.ANIMATE_COMPLETE, function(d, e)
		if oldcase then oldcase:removeSelf() end
		self.case = newcase
	end)
	self:addChild(newcase)
	if oldcase then oldcase:animate({x = self.w}, 1, "outBounce") end
end

function M:next()
	self.index = self.index + 1
	if self.index > #self.cases then
		self.index = #self.cases
		self.case:setX(-self.w / 8):animate({x = 0}, 0.5, "outBounce")
		return
	end
	
	local newcase = self.cases[self.index].new()
	local oldcase = self.case

	newcase:setPosition(self.w, 0)
	newcase:addEventListener(Event.ANIMATE_COMPLETE, function(d, e)
		if oldcase then oldcase:removeSelf() end
		self.case = newcase
	end)
	newcase:animate({x = 0}, 1, "outBounce")
	if oldcase then oldcase:animate({x = -self.w}, 1, "outBounce") end
	self:addChild(newcase)
end

return M
