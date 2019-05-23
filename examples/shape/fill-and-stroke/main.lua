local Shape = Shape
local M_PI = math.pi

local s = DisplayShape.new(stage:getSize())
	:moveTo(128.0, 25.6)
	:lineTo(230.4, 230.4)
	:relLineTo(-102.4, 0.0)
	:curveTo(51.2, 230.4, 51.2, 128.0, 128.0, 128.0)
	:closePath()
	:moveTo(64.0, 25.6)
	:relLineTo(51.2, 51.2)
	:relLineTo(-51.2, 51.2)
	:relLineTo(-51.2, -51.2)
	:closePath()
	:setLineWidth(10.0)
	:setSourceColor(0, 0, 1)
	:fillPreserve()
	:setSourceColor(0, 0, 0)
	:stroke()

stage:addChild(s)
