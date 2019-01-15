local AnalogClock = require("AnalogClock")
local Dobject = Dobject

stage:addChild(AnalogClock.new():setAnchor(0.5, 0.5):setAlignment(Dobject.ALIGN_CENTER))
stage:layout()
stage:showfps(true)
