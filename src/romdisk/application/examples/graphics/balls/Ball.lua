local M = Class(DisplayObject)

function M:init(image, w, h)
	self.super:init()

	local assets = assets

	self.maxWidth = w
	self.maxHeight = h
	self.xdirection = 1
	self.ydirection = 1
	self.xspeed = math.random(40, 100) / 10
	self.yspeed = math.random(40, 100) / 10
	self.rspeed = math.random(-360, 360) * math.pi / 180 * 2

	self:setX(math.random(40, self.maxWidth - 40))
	self:setY(math.random(40, self.maxHeight - 40))
	self:addChild(assets:loadDisplay(image):setAnchor(0.5, 0.5))
	self:addEventListener(Event.ENTER_FRAME, self.onEnterFrame)
end

function M:onEnterFrame(e)
	local x = self:getX()
	local y = self:getY()

	x = x + (self.xspeed * self.xdirection)
	y = y + (self.yspeed * self.ydirection)

	if x < 40 then
		self.xdirection = 1
	end

	if x > self.maxWidth - 40 then
		self.xdirection = -1
	end

	if y < 40 then
		self.ydirection = 1
	end

	if y > self.maxHeight - 40 then
		self.ydirection = -1
	end

	self:setPosition(x, y)
	self:setRotation(self:getRotation() + self.rspeed)
end

return M
