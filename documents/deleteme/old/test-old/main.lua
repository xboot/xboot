local Bmfont = require "Bmfont"

local background = DisplayImage.new(Texture.new("background.png"))
stage:addChild(background)

local text = Bmfont.new("font.txt", "font.png", "1234567890")
text:setPosition(100,100)

text:setText("abcdefghijk")

stage:addChild(text)

local c = 0.001
local looptimer = Timer.new(0.1, 0, function(t, e)
	c = c + 0.001
	text:setText("time: " .. c)
end)
