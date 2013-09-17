local Animation = require ("Animation")

local background = DisplayImage:new(Texture.new("background.png"))
runtime:addChild(background)

local boy = Texture.new("boy.png")
local boylist = {
	DisplayImage:new(boy:region(72 * 0, 0, 72, 72)),
	DisplayImage:new(boy:region(72 * 1, 0, 72, 72)),
	DisplayImage:new(boy:region(72 * 2, 0, 72, 72)),
	DisplayImage:new(boy:region(72 * 3, 0, 72, 72)),
	DisplayImage:new(boy:region(72 * 4, 0, 72, 72)),
	DisplayImage:new(boy:region(72 * 5, 0, 72, 72)),
	DisplayImage:new(boy:region(72 * 6, 0, 72, 72)),
	DisplayImage:new(boy:region(72 * 7, 0, 72, 72)),
	DisplayImage:new(boy:region(72 * 8, 0, 72, 72)),
	DisplayImage:new(boy:region(72 * 9, 0, 72, 72)),
	DisplayImage:new(boy:region(72 * 10, 0, 72, 72)),
}

local anmi = Animation:new(boylist, 100 ,100)
runtime:addChild(anmi)