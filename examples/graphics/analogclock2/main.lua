local AnalogClock = require("AnalogClock")

local sw, sh = stage:getSize()
stage:addChild(AnalogClock.new():setAnchor(0.5, 0.5):setPosition(sw / 2, sh / 2))
