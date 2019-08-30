local DisplayText = DisplayText

local txt = DisplayText.new("Hello World", Color.new("skyblue"), "roboto", 40)
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

