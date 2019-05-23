local DisplayText = DisplayText

local font = Font.new("assets/fonts/Roboto-Regular.ttf", 50)
local txt = DisplayText.new(font, Pattern.color(1, 0.2, 0.2), "Hello World")
txt:setPosition(200, 200)
stage:addChild(txt)

local count = 0
stage:addTimer(Timer.new(0.01, 0, function(t)
	count = count + 0.001
	txt:setText("time: " .. string.format("%.3f", count))
end))

stage:addTimer(Timer.new(1, 0, function(t)
	local s = math.random() * 360 + 0
	txt:animate({rotation = s}, 1, "circ-out")
end))

