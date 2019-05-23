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

stage:addChild(DisplayShape.new(sw, sh)
	:setSource(Pattern.image(assets:loadImage("assets/images/bg.png")):setExtend("repeat"))
	:paint())

for i = 1, 5 do
	local shape = DisplayShape.new(100, 50)
		:setLineWidth(6)
		:rectangle(0, 0, 100, 50)
		:setSourceColor(1, 0, 0, 0.5)
		:fillPreserve()
		:setSourceColor(0, 0, 0)
		:stroke()
		:setPosition(math.random(0, sw - 100), math.random(0, sh - 50))

	shape:addEventListener("mouse-down", onMouseDown)
	shape:addEventListener("mouse-move", onMouseMove)
	shape:addEventListener("mouse-up", onMouseUp)
	shape:addEventListener("touch-begin", onTouchBegin)
	shape:addEventListener("touch-move", onTouchMove)
	shape:addEventListener("touch-end", onTouchEnd)

	stage:addChild(shape)
end

