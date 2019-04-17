local stage = application:getStage()
local assets = application:getAssets()
local timermanager = application:getTimerManager()
local width, height = application:getScreenSize()

stage:addChild(DisplayShape.new(width, height):setSourceColor(0.8, 0.8, 0.8):paint())

local w = DisplayShape.new(400, 400):setAlignment(DisplayShape.ALIGN_NONE):setPosition(0, 0):setSourceColor(0.3, 0.3, 0.3):paint()

local w1 = DisplayShape.new(100, 100):setAlignment(DisplayShape.ALIGN_TOP):setPosition(100, 100):setSourceColor(0.7, 0, 0):paint()
local w1s1 = DisplayShape.new(50, 50):setAlignment(DisplayShape.ALIGN_LEFT):setPosition(20, 20):setSourceColor(0.3, 0.8, 0.8):paint()
local w1s2 = DisplayShape.new(50, 50):setAlignment(DisplayShape.ALIGN_RIGHT):setPosition(70, 70):setSourceColor(0.5, 0.7, 0.2):paint()
w1:addChild(w1s1)
w1:addChild(w1s2)

local w2 = DisplayShape.new(100, 100):setAlignment(DisplayShape.ALIGN_CENTER):setPosition(200, 200):setSourceColor(0, 0.7, 0):paint()
w:addChild(w1)
w:addChild(w2)

stage:addChild(w)

-- Create the Button
local button = Widget.Button.new({x = 200, y = 400, width = 100, height = 50})
	:addEventListener("click",
	function(d, e)
		print("Button [Click]")
	end)
stage:addChild(button)
