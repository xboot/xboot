local Dobject = require "graphic.dobject"
local AnalogClock = require("AnalogClock")

stage:addChild(AnalogClock.new():setAnchor(0.5, 0.5):setAlignment(Dobject.ALIGN_CENTER))
stage:layout()
stage:showfps(true)
