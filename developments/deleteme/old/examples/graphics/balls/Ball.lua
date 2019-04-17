local M = Class(DisplayObject)

function M:init(texture)
	DisplayObject.init(self)

	local bitmap = DisplayImage.new(texture)
	self:addChild(bitmap)

	self.xdirection = 1
	self.ydirection = 1
	self.xspeed = math.random(40, 100) / 10
	self.yspeed = math.random(40, 100) / 10

	self:setX(math.random(0, 800 - 80))
	self:setY(math.random(0, 480 - 80))

	self:addEventListener("enter-frame", self.onEnterFrame, self)
end

function M:onEnterFrame(e)
	local x = self:getX()
	local y = self:getY()

	x = x + (self.xspeed * self.xdirection)
	y = y + (self.yspeed * self.ydirection)

	if x < 0 then
		self.xdirection = 1
	end

	if x > 800 - self:getWidth() then
		self.xdirection = -1
	end

	if y < 0 then
		self.ydirection = 1
	end

	if y > 480 - self:getHeight() then
		self.ydirection = -1
	end

	self:setPosition(x, y)
end

return M
