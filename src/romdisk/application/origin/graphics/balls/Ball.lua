local M = Class(DisplayObject)

function M:init(image, maxWidth, maxHeight)
	self.super:init()

	self.image = assets:loadDisplay(image)
	local w, h = self.image:getSize()
	self.imgWidth = w
	self.imgHeight = h
	self.maxWidth = maxWidth
	self.maxHeight = maxHeight
	self.xdirection = 1
	self.ydirection = 1
	self.xspeed = math.random() * 2 + 1
	self.yspeed = math.random() * 2 + 1
	self.rspeed = math.random(0, 360) * math.pi / 180

	self:setX(math.random(self.imgWidth / 2, self.maxWidth - self.imgWidth / 2))
	self:setY(math.random(self.imgHeight / 2, self.maxHeight - self.imgHeight / 2))
	self:setSize(self.imgWidth, self.imgHeight)
	self:addChild(self.image)
	self:setAnchor(0.5, 0.5)
	self:addEventListener("enter-frame", self.onEnterFrame)
end

function M:setSize(width, height)
	self.super:setSize(width, height)
	return self
end

function M:onEnterFrame(e)
	local x = self:getX()
	local y = self:getY()

	x = x + (self.xspeed * self.xdirection)
	y = y + (self.yspeed * self.ydirection)

	if x < self.imgWidth / 2 then
		self.xdirection = 1
	end

	if x > self.maxWidth - self.imgWidth / 2 then
		self.xdirection = -1
	end

	if y < self.imgHeight / 2 then
		self.ydirection = 1
	end

	if y > self.maxHeight - self.imgHeight / 2 then
		self.ydirection = -1
	end

	self:setPosition(x, y)
	self:setRotation(self:getRotation() + self.rspeed)
end

return M
