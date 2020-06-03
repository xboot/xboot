local DisplayIcon = DisplayIcon

local ico = DisplayIcon.new(0xf067, Color.new("skyblue"), "font-awesome", 64, 64)
ico:setPosition(200, 200):setAnchor(0.5, 0.5)
stage:addChild(ico)

stage:addTimer(Timer.new(1, 0, function(t)
	local s = math.random() * 360 + 0
	ico:animate({rotation = s}, 1, "circ-out")
end))

