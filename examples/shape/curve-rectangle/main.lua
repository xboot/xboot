local M_PI = math.pi
local x0 = 25.6
local y0 = 25.6
local rect_width = 204.8
local rect_height = 204.8
local radius = 102.4
local x1 = x0 + rect_width
local y1 = y0 + rect_height

local s = DisplayShape.new(stage:getSize())
	:moveTo(x0, (y0 + y1) / 2)
	:curveTo(x0, y0, x0, y0, (x0 + x1) / 2, y0)
	:curveTo(x1, y0, x1, y0, x1, (y0 + y1) / 2)
	:curveTo(x1, y1, x1, y1, (x1 + x0) / 2, y1)
	:curveTo(x0, y1, x0, y1, x0, (y0 + y1) / 2)
	:closePath()
	:setSourceColor(0.5, 0.5, 1)
	:fillPreserve()
	:setSourceColor(0.5, 0, 0, 0.5)
	:setLineWidth(10.0)
	:stroke()

stage:addChild(s)
