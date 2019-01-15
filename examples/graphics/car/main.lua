local Dobject = Dobject
local Pattern = Pattern
local M_PI = math.pi

local sw, sh = stage:getSize()

stage:addChild(DisplayShape.new(sw, sh)
		:setSource(Pattern.image(assets:loadImage("bg.png")):setExtend(Pattern.EXTEND_REPEAT))
		:paint())

local car0 = assets:loadDisplay("car.png"):setPosition(0, 48 * 0)
local car1 = assets:loadDisplay("car.png"):setPosition(0, 48 * 1)
local car2 = assets:loadDisplay("car.png"):setPosition(0, 48 * 2)
local car3 = assets:loadDisplay("car.png"):setPosition(0, 48 * 3)
local car4 = assets:loadDisplay("car.png"):setPosition(0, 48 * 4)
local car5 = assets:loadDisplay("car.png"):setPosition(0, 48 * 5)
local car6 = assets:loadDisplay("car.png"):setPosition(0, 48 * 6)
local car7 = assets:loadDisplay("car.png"):setPosition(0, 48 * 7)
local car8 = assets:loadDisplay("car.png"):setPosition(0, 48 * 8)
local car9 = assets:loadDisplay("car.png"):setPosition(0, 48 * 9)

stage:addTimer(Timer.new(1.5, 0, function(t)
	car0:setPosition(0, 48 * 0):animate({x = sw - 98, y = 48 * 0}, 1, "linear")
	car1:setPosition(0, 48 * 1):animate({x = sw - 98, y = 48 * 1}, 1, "inSine")
	car2:setPosition(0, 48 * 2):animate({x = sw - 98, y = 48 * 2}, 1, "outSine")
	car3:setPosition(0, 48 * 3):animate({x = sw - 98, y = 48 * 3}, 1, "inOutSine")
	car4:setPosition(0, 48 * 4):animate({x = sw - 98, y = 48 * 4}, 1, "inQuad")
	car5:setPosition(0, 48 * 5):animate({x = sw - 98, y = 48 * 5}, 1, "outQuad")
	car6:setPosition(0, 48 * 6):animate({x = sw - 98, y = 48 * 6}, 1, "inOutQuad")
	car7:setPosition(0, 48 * 7):animate({x = sw - 98, y = 48 * 7}, 1, "inCubic")
	car8:setPosition(0, 48 * 8):animate({x = sw - 98, y = 48 * 8}, 1, "outCubic")
	car9:setPosition(0, 48 * 9):animate({x = sw - 98, y = 48 * 9}, 1, "inOutCubic")
end))

stage:addChild(car0)
stage:addChild(car1)
stage:addChild(car2)
stage:addChild(car3)
stage:addChild(car4)
stage:addChild(car5)
stage:addChild(car6)
stage:addChild(car7)
stage:addChild(car8)
stage:addChild(car9)

stage:showfps(true)
