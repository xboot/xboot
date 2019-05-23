local Shape = Shape
local M_PI = math.pi
local sqrt = math.sqrt

local s = DisplayShape.new(stage:getSize())
	:setLineWidth(40.96)
	:moveTo(76.8, 84.48)
	:relLineTo(51.2, -51.2)
	:relLineTo(51.2, 51.2)
	:setLineJoin("miter")
	:stroke()
	:moveTo(76.8, 161.28)
	:relLineTo(51.2, -51.2)
	:relLineTo(51.2, 51.2)
	:setLineJoin("bevel")
	:stroke()
	:moveTo(76.8, 238.08)
	:relLineTo(51.2, -51.2)
	:relLineTo(51.2, 51.2)
	:setLineJoin("round")
	:stroke()

stage:addChild(s)
