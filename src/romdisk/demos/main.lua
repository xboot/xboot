local TestCase = require("TestCase")

local stage = application:getStage()
local assets = application:getAssets()
local width, height = application:getScreenSize()
application:setBackgroundColor(0.8, 0.8, 0.8) 

local testcases = TestCase.new({
	require("scene1"),
	require("scene2"),	
	require("widgets.button"),
	require("graphics.balls"),
})
stage:addChild(testcases)

local prevbtn = Widget.Button.new({x = 50, y = height - 100, width = 100, height = 50})
	:addEventListener("Click", function(d, e) testcases:prev() end)
stage:addChild(prevbtn)

local nextbtn = Widget.Button.new({x = width - 150, y = height - 100, width = 100, height = 50})
	:addEventListener("Click", function(d, e) testcases:next() end)
stage:addChild(nextbtn)
