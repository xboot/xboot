local Sun = require "Sun"

local sw, sh = stage:getSize()
stage:addChild(DisplayImage.new(Image.new(sw, sh)
	:save()
	:setSource(Pattern.image(Image.new("assets/images/bg.png")):setExtend("reflect"))
	:paint()
	:restore()))
stage:addChild(Sun.new(sw / 2, sh / 4))
