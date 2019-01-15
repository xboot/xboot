local Shape = Shape
local M_PI = math.pi
local sqrt = math.sqrt

local s = DisplayShape.new(stage:getSize())
	:setLineWidth(40.96)
	:moveTo(76.8, 84.48)
	:relLineTo(51.2, -51.2)
	:relLineTo(51.2, 51.2)
	:setLineJoin(Shape.LINE_JOIN_MITER)
	:stroke()
	:moveTo(76.8, 161.28)
	:relLineTo(51.2, -51.2)
	:relLineTo(51.2, 51.2)
	:setLineJoin(Shape.LINE_JOIN_BEVEL)
	:stroke()
	:moveTo(76.8, 238.08)
	:relLineTo(51.2, -51.2)
	:relLineTo(51.2, 51.2)
	:setLineJoin(Shape.LINE_JOIN_ROUND)
	:stroke()

stage:addChild(s)
stage:showfps(true)
