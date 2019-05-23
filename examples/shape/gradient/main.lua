local Pattern = Pattern
local Shape = Shape
local M_PI = math.pi

local p1 = Pattern.linear(0.0, 0.0, 0.0, 256.0)
	:addColorStop(1, 0, 0, 0, 1)
	:addColorStop(0, 1, 1, 1, 1)

local s1 = DisplayShape.new(stage:getSize())
	:rectangle(0, 0, 256, 256)
	:setSource(p1)
	:fill()

local p2 = Pattern.radial(115.2, 102.4, 25.6, 102.4,  102.4, 128.0)
	:addColorStop(0, 1, 1, 1, 1)
	:addColorStop(1, 0, 0, 0, 1)

local s2 = DisplayShape.new(stage:getSize())
	:setSource(p2)
	:arc(128.0, 128.0, 76.8, 0, 2 * M_PI)
	:fill()

stage:addChild(s1)
stage:addChild(s2)

