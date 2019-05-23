local Wood = require "Wood"

local sw, sh = stage:getSize()

local wood = Wood.new():setAnchor(0.5, 0.5):setPosition(sw / 2, 120)
local knife = assets:loadDisplay("knife.png"):setAnchor(0.5, 0):setPosition(sw / 2, sh - 120):setVisible(true)
local dropknife = assets:loadDisplay("knife.png"):setAnchor(0.5, 0.5):setPosition(sw / 2, 120):setVisible(false)
local isready = true

dropknife:addEventListener("animate-complete", function(d, e)
	dropknife:setVisible(false)
	knife:setVisible(true)
	wood:drop()
	isready = true
end)

knife:addEventListener("animate-complete", function(d, e)
	if wood:hitting() then
		knife:setVisible(false):setPosition(sw / 2, sh - 120)
		dropknife:setVisible(true):setPosition(sw / 2, 120):animate({x = sw / 2, y = sh - 120, rotation = dropknife:getRotation() + 180}, 0.6, "sine-in")
		wood:animate({scalex = 1.1, scaley = 1.1}, 0.3, "bounce-out"):animate({scalex = 1, scaley = 1}, 0.3, "bounce-in")
	else
		knife:setPosition(sw / 2, sh - 120)
		isready = true
	end
end)

local function onEnter(d, e)
	if isready then
		isready = false
		d:setPosition(sw / 2, sh - 120):animate({x = sw / 2, y = 120}, 0.3, "sine-in")
	end
end

stage:addEventListener("mouse-down", onEnter, knife)
stage:addEventListener("touch-begin", onEnter, knife)
stage:addEventListener("key-down", onEnter, knife)

stage:addChild(knife)
stage:addChild(dropknife)
stage:addChild(wood)

