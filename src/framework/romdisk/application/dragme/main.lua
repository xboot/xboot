local function onMouseDown(self, e)
	if self:hitTestPoint(e.x, e.y) then
		self.touchid = -1
		self.x0 = e.x
		self.y0 = e.y
		e.stop = true
	end
end

local function onMouseMove(self, e)
	if self.touchid == -1 then
		local x1, y1 = self:globalToLocal(self.x0, self.y0)
		local x2, y2 = self:globalToLocal(e.x, e.y)
		self:setX(self:getX() + x2 - x1)
		self:setY(self:getY() + y2 - y1)
		self.x0 = e.x
		self.y0 = e.y
		e.stop = true
	end
end

local function onMouseUp(self, e)
	if self.touchid == -1 then
		self.touchid = nil
		e.stop = true
	end
end

local function onTouchBegin(self, e)
	if self:hitTestPoint(e.x, e.y) then
		self.touchid = e.id
		self.x0 = e.x
		self.y0 = e.y
		e.stop = true
	end
end

local function onTouchMove(self, e)
	if self.touchid == e.id then
		local x1, y1 = self:globalToLocal(self.x0, self.y0)
		local x2, y2 = self:globalToLocal(e.x, e.y)
		self:setX(self:getX() + x2 - x1)
		self:setY(self:getY() + y2 - y1)
		self.x0 = e.x
		self.y0 = e.y
		e.stop = true
	end
end

local function onTouchEnd(self, e)
	if self.touchid == e.id then
		self.touchid = nil
		e.stop = true
	end
end

local sw, sh = stage:getSize()
stage:addChild(DisplayImage.new(Image.new("assets/images/bg.png"):extend(sw, sh, "repeat")))

for i = 1, 5 do
	local image = DisplayImage.new(Image.new(100, 50)
			:rectangle(0, 0, 100, 50, 0, -1, Color.new("#FF000080"))
			:rectangle(0, 0, 100, 50, 0, 6, Color.new("#282828FF"))
		):setPosition(math.random(0, sw - 100), math.random(0, sh - 50))

	image:addEventListener("mouse-down", onMouseDown)
	image:addEventListener("mouse-move", onMouseMove)
	image:addEventListener("mouse-up", onMouseUp)
	image:addEventListener("touch-begin", onTouchBegin)
	image:addEventListener("touch-move", onTouchMove)
	image:addEventListener("touch-end", onTouchEnd)

	stage:addChild(image)
end
