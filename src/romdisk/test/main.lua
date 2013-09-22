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
anmi:setAnchor(0.5, 0.5)
runtime:addChild(anmi)

local font = Font.new("/romdisk/system/media/fonts/DroidSansFallback.ttf")
local text = DisplayText:new(font, "我能吞下玻璃，123")
text:setPosition(100,100)
text:setScale(24, 24)
runtime:addChild(text)

Timer:new(1 / 5, 0, function(t, e)
	anmi:rotate(10)
end)
