local DisplayIcon = DisplayIcon

local ico = DisplayIcon.new(0xe900, Color.new("skyblue"), "bootstrap-icons", 64)
ico:setPosition(200, 200):setAnchor(0.5, 0.5)
stage:addChild(ico)

stage:addTimer(Timer.new(1, 0, function(t)
	local r = math.random() * 360 + 0
	local s = math.random() * 5
	ico:animate({rotation = r, scalex = s, scaley = s}, 1, "circ-out")
end))

