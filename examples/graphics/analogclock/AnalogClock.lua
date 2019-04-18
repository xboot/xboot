local M = Class(DisplayObject)
local os = os

function M:init()
	self.super:init()

	local face = assets:loadDisplay("clock-face.png")
	local width, height = face:getSize()
	
	local hour = assets:loadDisplay("clock-hour-hand.png")
	hour:setPosition(width / 2, height /2)
		:setAnchor(0.5, 0.9)
		:setRotation(0)

	local minute = assets:loadDisplay("clock-minute-hand.png")
	minute:setPosition(width / 2, height /2)
		:setAnchor(0.5, 0.9)
		:setRotation(0)

	local second = assets:loadDisplay("clock-second-hand.png")
	second:setPosition(width / 2, height /2)
		:setAnchor(0.5, 0.766839378)
		:setRotation(0)

	local oval = assets:loadDisplay("clock-oval.png")
	oval:setPosition(width / 2, height /2)
		:setAnchor(0.5, 0.5)
		:setRotation(0)

	self:setSize(width, height)
	self:addChild(face)
	self:addChild(hour)
	self:addChild(minute)
	self:addChild(second)
	self:addChild(oval)

	local time = os.date("*t");
	hour:animate({rotation = time.hour * 360 / 12 + time.min * 360 / (12 * 60) + time.sec * 360 / (12 * 60 * 60)}, 0.5, "cubic-in-out")
	minute:animate({rotation = time.min * 360 / 60 + time.sec * 360 / (60 * 60)}, 0.5, "cubic-in-out")
	second:animate({rotation = time.sec * 360 / 60}, 0.5, "cubic-in-out")

	stage:addTimer(Timer.new(1, 0, function(t)
		local time = os.date("*t");
		hour:setRotation(time.hour * 360 / 12 + time.min * 360 / (12 * 60) + time.sec * 360 / (12 * 60 * 60))
		minute:setRotation(time.min * 360 / 60 + time.sec * 360 / (60 * 60))
		second:setRotation(time.sec * 360 / 60)
	end))
end

function M:setSize(width, height)
	self.super:setSize(width, height)
	return self
end

return M
