local Shape = Shape
local M_PI = math.pi
local sqrt = math.sqrt

local s = DisplayShape.new(stage:getSize())
	:setLineWidth(30.0)
	:setLineCap("butt")
	:moveTo(64.0, 50.0)
	:lineTo(64.0, 200.0)
	:stroke()
	:setLineCap("round")
	:moveTo(128.0, 50.0)
	:lineTo(128.0, 200.0)
	:stroke()
	:setLineCap("square")
	:moveTo(192.0, 50.0)
	:lineTo(192.0, 200.0)
	:stroke()
	:setSourceColor(1, 0.2, 0.2)
	:setLineWidth(2.56)
	:moveTo(64.0, 50.0)
	:lineTo(64.0, 200.0)
	:moveTo(128.0, 50.0)
	:lineTo(128.0, 200.0)
	:moveTo(192.0, 50.0)
	:lineTo(192.0, 200.0)
	:stroke()

stage:addChild(s)
