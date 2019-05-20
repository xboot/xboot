local Easing = Easing

local M = Class(DisplayObject)

function M:init(width, height, cases)
	self.super:init(width, height)

	self.width = width or 640
	self.height = height or 480
	self.cases = cases or {}
	self.index = 1
	if #self.cases > 0 then
		self.case1 = self.cases[self.index].new(self.width, self.height)
		self.case2 = nil
		self:addChild(self.case1)
	end
	self.tweening = false
	
	self:addEventListener("enter-frame", self.onEnterFrame)
end

function M:onEnterFrame(e)
	if not self.tweening then
		return
	end

	local elapsed = self.watch:elapsed()
	if elapsed < self.duration then
		self.transition(elapsed)
	else
		self.transition(self.duration)
		self:removeChild(self.case1)
		self.case1 = self.case2
		self.case2 = nil
		self.transition = nil
		self.ease = nil
		self.watch = nil
		self.tweening = false

		collectgarbage()
	end
end

function M:select(index, duration, transition, ease)
	if self.tweening then
		return
	end

	if index < 1 then
		index = 1;
	elseif index > #self.cases then
		index = #self.cases
	end
	
	if index == self.index then
		return
	end
	self.index = index
	
	if self.case1 == nil then
		self.case1 = self.cases[self.index].new(self.width, self.height)
		self:addChild(self.case)
		return
	end
	self.case2 = self.cases[self.index].new(self.width, self.height)
	self:addChild(self.case2)

	self.duration = duration or 1
	local transition = transition or "move-from-left"
	local ease = ease or "bounce-out"

	if transition == "move-from-left" then
		self.transition = function(t)
			local x = self.ease(t)
			self.case1:setX(x)
			self.case2:setX(x - self.width)
		end
		self.ease = Easing.new(0, self.width, self.duration, ease)

	elseif transition == "move-from-right" then
		self.transition = function(t)
			local x = self.ease(t)
			self.case1:setX(-x)
			self.case2:setX(self.width - x)
		end
		self.ease = Easing.new(0, self.width, self.duration, ease)

	elseif transition == "move-from-top" then
		self.transition = function(t)
			local y = self.ease(t)
			self.case1:setY(y)
			self.case2:setY(y - self.height)
		end
		self.ease = Easing.new(0, self.height, self.duration, ease)
	
	elseif transition == "move-from-bottom" then
		self.transition = function(t)
			local y = self.ease(t)
			self.case1:setY(-y)
			self.case2:setY(self.height - y)
		end
		self.ease = Easing.new(0, self.height, self.duration, ease)
	
	elseif transition == "over-from-left" then
		self.transition = function(t)
			local x = self.ease(t)
			self.case2:setX(x - self.width)
		end
		self.ease = Easing.new(0, self.width, self.duration, ease)
		
	elseif transition == "over-from-right" then
		self.transition = function(t)
			local x = self.ease(t)
			self.case2:setX(self.width - x)
		end
		self.ease = Easing.new(0, self.width, self.duration, ease)

	elseif transition == "over-from-top" then
		self.transition = function(t)
			local y = self.ease(t)
			self.case2:setY(y - self.height)
		end
		self.ease = Easing.new(0, self.height, self.duration, ease)
	
	elseif transition == "over-from-bottom" then
		self.transition = function(t)
			local y = self.ease(t)
			self.case2:setY(self.height - y)
		end
		self.ease = Easing.new(0, self.height, self.duration, ease)
	end

	self.watch = Stopwatch.new()
	self.tweening = true
end

function M:prev()
	self:select(self.index - 1, 0.6, "move-from-left", "bounce-out")
end

function M:next()
	self:select(self.index + 1, 0.6, "move-from-right", "bounce-out")
end

return M
