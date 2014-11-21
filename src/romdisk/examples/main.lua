local TestCase = require("TestCase")

local stage = application:getStage()
local width, height = application:getScreenSize()

local testcases = TestCase.new({
	require("games.2048"),
	require("graphics.balls"),
	require("graphics.cursor"),
	require("graphics.dragme"),
	require("widgets.button"),
	require("widgets.checkbox"),
	require("widgets.radiobutton"),
})
stage:addChild(testcases)

local prevbtn = Widget.Button.new({x = 50, y = height - 100, width = 100, height = 50})
	:addEventListener("Click", function(d, e) testcases:prev() end)
stage:addChild(prevbtn)

local nextbtn = Widget.Button.new({x = width - 150, y = height - 100, width = 100, height = 50})
	:addEventListener("Click", function(d, e) testcases:next() end)
stage:addChild(nextbtn)