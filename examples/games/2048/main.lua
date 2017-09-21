local Game = require("Game")
local stage = Stage.new()

stage:addChild(Game.new():setAnchor(0.5, 0.5):setAlignment(Object.ALIGN_CENTER))
stage:layout()
stage:showfps(true)

stage:loop()