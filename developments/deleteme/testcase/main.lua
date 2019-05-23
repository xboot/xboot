local Button = require "xboot.widget.Button"
local TestCase = require("TestCase")

local sw, sh= stage:getSize()

local testcases = TestCase.new(sw, sh, {
	require("games.2048"),
	require("graphics.balls"),
	require("graphics.cursor"),
	require("graphics.dragme"),
	require("widgets.button"),
	require("widgets.checkbox"),
	require("widgets.radiobutton"),
})
stage:addChild(testcases)

local prevbtn = Button.new({x = 50, y = sh - 100, width = 100, height = 50})
	:addEventListener("click", function(d, e) testcases:prev() end)
stage:addChild(prevbtn)

local nextbtn = Button.new({x = sw - 150, y = sh - 100, width = 100, height = 50})
	:addEventListener("click", function(d, e) testcases:next() end)
stage:addChild(nextbtn)

stage:addEventListener("key-down", function(d, e)
	local key = e.key
	if key == 2 or key == 4 or key == 6 then
		testcases:prev()
	elseif key == 3 or key == 5 or key == 7 then
		testcases:next()
	else
		return
	end
	e.stop = true
end)

