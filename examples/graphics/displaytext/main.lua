local DisplayText = DisplayText
local sw, sh = stage:getSize()

local font = Font.new("assets/fonts/Roboto-Regular.ttf")
local txt = DisplayText.new(font, 72, Pattern.color(1, 0.2, 0.2), "Hello World")
txt:setPosition(100, 100):setScale(50)
stage:addChild(txt)

local count = 0
stage:addTimer(Timer.new(0.01, 0, function(t)
	count = count + 0.001
	txt:setText("time: " .. string.format("%.3f", count))
end))

stage:showfps(true)