local TestCase = require("TestCase")

local stage = Stage.new()
local sw, sh= stage:getSize()

local testcases = TestCase.new({
	require("games.2048"),
	require("graphics.balls"),
	require("graphics.cursor"),
	require("graphics.dragme"),
	require("widgets.button"),
	require("widgets.checkbox"),
	require("widgets.radiobutton"),
}, sw, sh)
stage:addChild(testcases)

local prevbtn = Widget.Button.new({x = 50, y = sh - 100, width = 100, height = 50})
	:addEventListener("Click", function(d, e) testcases:prev() end)
stage:addChild(prevbtn)

local nextbtn = Widget.Button.new({x = sw - 150, y = sh - 100, width = 100, height = 50})
	:addEventListener("Click", function(d, e) testcases:next() end)
stage:addChild(nextbtn)

stage:addEventListener(Event.KEY_DOWN, function(d, e)
  local key = e.key
  if key == 0x93 or key == 0x95 or key == 0x9c then
    testcases:prev()
  elseif key == 0x94 or key == 0x96 or key == 0x9b then
    testcases:next()
  else
    return
  end
  e.stop = true
end)

stage:showfps(true)
stage:loop()