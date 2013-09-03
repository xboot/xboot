local M = class(display_object)

function M:init(texture)
	display_object.init(self)

	local bitmap = display_image:new(texture)
	self:add_child(bitmap)

	self.xdirection = 1
	self.ydirection = 1
	self.xspeed = math.random(40, 100) / 10
	self.yspeed = math.random(40, 100) / 10

	self:setx(math.random(0, 800 - 80))
	self:sety(math.random(0, 480 - 80))

	self.width = bitmap.width
	self.height = bitmap.height
	
	self:add_event_listener(event.ENTER_FRAME, self.on_enter_frame, self)
end

function M:on_enter_frame(e)
	local x = self.x
	local y = self.y

	x = x + (self.xspeed * self.xdirection)
	y = y + (self.yspeed * self.ydirection)

	if x < 0 then
		self.xdirection = 1
	end

	if x > 800 - self.width then
		self.xdirection = -1
	end

	if y < 0 then
		self.ydirection = 1
	end

	if y > 480 - self.height then
		self.ydirection = -1
	end

	self:setxy(x, y)
end

return M
