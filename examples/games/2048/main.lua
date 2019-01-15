local Dobject = Dobject
local Game = require("Game")

stage:addChild(Game.new():setAnchor(0.5, 0.5):setAlignment(Dobject.ALIGN_CENTER))
stage:layout()
stage:showfps(true)
