local Pattern = Pattern
local M_PI = math.pi

local sw, sh = stage:getSize()

stage:addChild(DisplayShape.new(sw, sh)
		:setSource(Pattern.image(assets:loadImage("bg.png")):setExtend("repeat"))
		:paint())

local a = assets:loadDisplay("planet-earth.png"):setPosition(sw / 2, sh / 2):setScale(0.5):setAnchor(0.5, 0.5)
stage:addChild(a)

local s = nil
local count = 0;
local function onEnterFrame(d, e)
	count = count + 0.1

	stage:removeChild(s)
	s = nil

	s = DisplayShape.new(sw / 2, sh / 2)
		:setAnchor(0.5, 0.5)		
		:setPosition(sw / 2, sh / 2)

	s:setLineWidth(10)
		:moveTo(40 + math.sin(count) * 40, 40 + math.cos(count) * 40)
		:lineTo(260 + math.cos(count) * 40, 40 + math.sin(count) * 40)
		:lineTo(260 + math.sin(count) * 40, 200 + math.cos(count) * 40)
		:lineTo(40 + math.cos(count)* 40, 200 + math.sin(count) * 40)
		:lineTo(40 + math.sin(count) * 40, 40 + math.cos(count) * 40)
		:closePath()
		:setSourceColor(1, 1, 0, 0.5)
		:fillPreserve()
		:setSourceColor(1, 0, 0, 1)
		:stroke()

	a:setRotation(a:getRotation() + 1)
	s:setRotation(count * 0.02 * 360)
	stage:addChild(s)
end

stage:addEventListener("enter-frame", onEnterFrame)
