local math = require "math"

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

local xc = 128.0
local yc = 128.0
local radius = 100.0
local angle1 = 45.0 * (math.pi / 180.0)
local angle2 = 180.0 * (math.pi / 180.0)

local panel = Image.new(256, 256)
	:shapeSave()
	:shapeSetLineWidth(1)
	:shapeSetSourceColor(Color.new("blue"))
	:shapeRectangle(0, 0, 256, 256)
	:shapeStroke()
	:shapeSetLineWidth(10.0)
	:shapeSetSourceColor(Color.new("black"))
	:shapeArc(xc, yc, radius, angle1, angle2)
	:shapeStroke()
	:shapeSetSourceColor(Color.new("red"))
	:shapeSetLineWidth(6.0)
	:shapeArc(xc, yc, 10.0, 0, 2 * math.pi)
	:shapeFill()
	:shapeArc(xc, yc, radius, angle1, angle1)
	:shapeLineTo(xc, yc)
	:shapeArc(xc, yc, radius, angle2, angle2)
	:shapeLineTo(xc, yc)
	:shapeStroke()
	:shapeRestore()

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

