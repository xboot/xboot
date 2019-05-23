local Sun = require "Sun"

local sw, sh = stage:getSize()
stage:addChild(DisplayShape.new(sw, sh)
	:setSource(Pattern.image(assets:loadImage("assets/images/bg.png")):setExtend("reflect"))
	:paint())
stage:addChild(Sun.new(sw / 2, sh / 4))

