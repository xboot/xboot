local Game = require("Game")

stage:addChild(Game.new():setLayoutable(true))
stage:setLayoutDirection("row"):setLayoutJustify("center"):setLayoutAlign("center")
stage:layout()
stage:showfps(true)
