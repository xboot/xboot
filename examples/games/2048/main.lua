local Game = require("Game")

stage:addChild(Game.new())
stage:setLayoutDirection("row"):setLayoutJustify("center"):setLayoutAlign("center"):setLayoutEnable(true)
stage:layout()
stage:showfps(true)
