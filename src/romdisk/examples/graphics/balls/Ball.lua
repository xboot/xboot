local M = Class(DisplayObject)

function M:init(image)
	self.super:init()

	local width, height = application:getScreenSize()
	local assets = application:getAssets()

	self.maxWidth = width
	self.maxHeight = height
	self.xdirection = 1
	self.ydirection = 1
	self.xspeed = math.random(40, 100) / 10
	self.yspeed = math.random(40, 100) / 10

	self:setX(math.random(0, self.maxWidth - 80))
	self:setY(math.random(0, self.maxHeight - 80))
	self:addChild(assets:loadDisplay(image))
	self:addEventListener(Event.ENTER_FRAME, self.onEnterFrame, self)
end

function M:onEnterFrame(e)
	local x = self:getX()
	local y = self:getY()

	x = x + (self.xspeed * self.xdirection)
	y = y + (self.yspeed * self.ydirection)

	if x < 0 then
		self.xdirection = 1
	end

	if x > self.maxWidth - 80 then
		self.xdirection = -1
	end

	if y < 0 then
		self.ydirection = 1
	end

	if y > self.maxHeight - 80 then
		self.ydirection = -1
	end

	self:setPosition(x, y)
end

return M
