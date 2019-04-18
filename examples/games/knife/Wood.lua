local DisplayObject = DisplayObject
local stage = stage

local M = Class(DisplayObject)

function M:init()
	self.super:init()

	self.imgWood = assets:loadImage("wood.png")
	local w, h = self.imgWood:getSize()
	self.imgWoodWidth = w
	self.imgWoodHeight = h

	self.imgKnife = assets:loadImage("knife.png")
	local w, h = self.imgKnife:getSize()
	self.imgKnifeWidth = w
	self.imgKnifeHeight = h

	self:setSize(self.imgWoodWidth * 2, self.imgWoodHeight * 2)
	self.wood = DisplayImage.new(self.imgWood):setAnchor(0.5, 0.5):setPosition(self.imgWoodWidth, self.imgWoodHeight)
	self:addChild(self.wood)
	self:addEventListener("enter-frame", self.onEnterFrame)
	self.knife = {}

	self.rspeed = math.random(-360, 360) / 90
	stage:addTimer(Timer.new(2, 0, function(t)
		local sign = math.random() > 0.5 and 1 or -1
		self.rspeed = (self.rspeed + math.random(-360, 360) / 90) * sign
	end))
end

function M:setSize(width, height)
	self.super:setSize(width, height)
	return self
end

function M:hitting()
	local knife = DisplayImage.new(self.imgKnife):setAnchor(0.5, 0):setPosition(self.imgWoodWidth, self.imgWoodHeight):setRotation(-self:getRotation())
	for i, v in ipairs(self.knife) do
		local angle = math.abs((v:getRotation() - knife:getRotation()) % 360)
		if angle < 18 then
			return true
		end
	end
	self:addChild(knife)
	knife:toBack()
	table.insert(self.knife, knife)
	return false
end

function M:drop()
	for i, v in ipairs(self.knife) do
		v:removeSelf()
	end
	self.knife = {}
end

function M:onEnterFrame(e)
	self:setRotation(self:getRotation() + self.rspeed)
end

return M
