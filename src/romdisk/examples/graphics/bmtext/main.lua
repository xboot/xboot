local background = DisplayImage:new(Texture:new("background.png"))
runtime:addChild(background)

local text = DisplayBmtext:new("simhei_regular_24", "123", 0, 0)
--text:setXY(100,100)
text:setText("abcdefg12345我是好人ABC")
runtime:addChild(text)

local c = 0.01
local looptimer = Timer:new(0.01, 0, function(t, e)
	c = c + 0.01
	text:setText("time: " .. c)
end)
