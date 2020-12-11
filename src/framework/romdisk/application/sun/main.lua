local Sun = require "Sun"

local sw, sh = stage:getSize()
stage:addChild(DisplayImage.new(Image.new("assets/images/bg.png"):extend(sw, sh, "repeat")))
stage:addChild(Sun.new(sw / 2, sh / 4))
