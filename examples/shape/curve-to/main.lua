local M_PI = math.pi
local x = 25.6
local y = 128.0
local x1 = 102.4
local y1 = 230.4
local x2 = 153.6
local y2 = 25.6
local x3 = 230.4
local y3 = 128.0

local s = DisplayShape.new(stage:getSize())
	:moveTo(x, y)
	:curveTo(x1, y1, x2, y2, x3, y3)
	:setLineWidth(10.0)
	:stroke()
	:setSourceColor(1, 0.2, 0.2, 0.6)
	:setLineWidth(6.0)
	:moveTo(x,y)
	:lineTo(x1,y1)
	:moveTo(x2,y2)
	:lineTo(x3,y3)
	:stroke()

stage:addChild(s)
