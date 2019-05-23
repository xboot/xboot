local Button = require "xboot.widget.Button"

local btn = Button.new({x = 100, y = 100, width = 100, height = 50})
	:addEventListener("press", function(d, e) print("Button [Press]") end)
	:addEventListener("release", function(d, e) print("Button [Release]") end)
	:addEventListener("click", function(d, e) print("Button [Click]") end)
stage:addChild(btn)

