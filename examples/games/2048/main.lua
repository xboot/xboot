local Game = require("Game")

local sw, sh = stage:getSize()
stage:addChild(Game.new():setAnchor(0.5, 0.5):setPosition(sw / 2, sh / 2))
