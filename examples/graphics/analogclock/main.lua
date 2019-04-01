local AnalogClock = require("AnalogClock")

stage:addChild(AnalogClock.new())
stage:setLayoutDirection("row"):setLayoutJustify("center"):setLayoutAlign("center"):setLayoutEnable(true)
stage:layout()
stage:showfps(true)
