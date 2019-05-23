local M_PI = math.pi
local xc = 128.0;
local yc = 128.0;
local radius = 100.0;
local angle1 = 45.0  * (M_PI / 180.0);
local angle2 = 180.0 * (M_PI / 180.0);

local s = DisplayShape.new(stage:getSize())
	:setLineWidth(10)
	:arcNegative(xc, yc, radius, angle1, angle2)
	:stroke()
	:setSourceColor(1, 0.2, 0.2, 0.6)
	:setLineWidth(6)
	:arc(xc, yc, 10.0, 0, 2 * M_PI)
	:fill()
    :arc(xc, yc, radius, angle1, angle1)
    :lineTo(xc, yc)
    :arc(xc, yc, radius, angle2, angle2)
    :lineTo(xc, yc)
    :stroke()

stage:addChild(s)
