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
		local dx = e.x - self.x0
		local dy = e.y - self.y0
		self:setX(self:getX() + dx)
		self:setY(self:getY() + dy)
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
		local dx = e.x - self.x0
		local dy = e.y - self.y0
		self:setX(self:getX() + dx)
		self:setY(self:getY() + dy)
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

local panel = Image.new(200, 200)
panel:rectangle(0, 0, 200, 200, 0, 1, Color.new("red"))
panel:circle(100, 100, 50, 10, Color.new("blue"))

local img = DisplayImage.new(panel)
	:setPosition(math.random(0, sw - 64), math.random(0, sh - 64))
	:setAnchor(0.5, 0.5)

img:addEventListener("mouse-down", onMouseDown)
img:addEventListener("mouse-move", onMouseMove)
img:addEventListener("mouse-up", onMouseUp)
img:addEventListener("touch-begin", onTouchBegin)
img:addEventListener("touch-move", onTouchMove)
img:addEventListener("touch-end", onTouchEnd)

stage:addChild(img)
