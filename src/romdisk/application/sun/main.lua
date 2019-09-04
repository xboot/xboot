local Sun = require "Sun"

local sw, sh = stage:getSize()
stage:addChild(DisplayImage.new(Image.new(sw, sh):extend(Image.new("assets/images/bg.png"), "repeat")))
stage:addChild(Sun.new(sw / 2, sh / 4))
