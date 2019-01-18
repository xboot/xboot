local Bmfont = require "Bmfont"

local background = DisplayImage.new(Image.new("background.png"))
stage:addChild(background)

local text = Bmfont.new("font.txt", "font.png", "1234567890")
text:setPosition(100,100)

text:setText("234")

stage:addChild(text)

local c = 0
stage:addTimer(Timer.new(0.01, 0, function(t)
	c = c + 0.001
	text:setText("time: " .. string.format("%.3f", c))
end))

stage:showfps(true)
