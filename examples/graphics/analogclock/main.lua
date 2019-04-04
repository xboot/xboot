local AnalogClock = require("AnalogClock")

stage:addChild(AnalogClock.new():setLayoutable(true))
stage:setLayoutDirection("row"):setLayoutJustify("center"):setLayoutAlign("center")
stage:showfps(true)
