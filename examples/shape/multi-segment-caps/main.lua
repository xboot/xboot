local Shape = Shape
local M_PI = math.pi
local sqrt = math.sqrt

local s = DisplayShape.new(stage:getSize())
	:moveTo(50.0, 75.0)
	:lineTo(200.0, 75.0)
	:moveTo(50.0, 125.0)
	:lineTo(200.0, 125.0)
	:moveTo(50.0, 175.0)
	:lineTo(200.0, 175.0)
	:setLineWidth(30.0)
	:setLineCap("round")
	:stroke()

stage:addChild(s)
