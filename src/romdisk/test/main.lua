local Animation = require ("Animation")

local background = DisplayImage:new(Texture.new("background.png"))
runtime:addChild(background)

local s = DisplayShape:new(1, 1)
runtime:addChild(s)

s:test()
s:setPosition(150, 150)
s:setAnchor(0.5, 0.5)
s:setRotation(45)
s:setScale(50, 50)

	
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
local text = DisplayText:new(font)
text:setPosition(100,100)
text:setScale(100, 100)
runtime:addChild(text)

--[[
local par = Parttern.linear(0, 15, 0, 90 * 0.8)
par:setExtend(Parttern.EXTEND_REPEAT)
par:addColor(0.0, 1, 0.6, 0)
par:addColor(0.5, 1, 0.3, 0)

text:setParttern(par)

local ttboy = Texture.new("ttboy.png")
local ttpar = ttboy:toPattern()
ttpar:setExtend(Parttern.EXTEND_REPEAT)
text:setParttern(ttpar)
]]

local i = 0;
Timer:new(1 / 5, 0, function(t, e)
	anmi:rotate(10)
	text:rotate(5)
	s:rotate(6)
	
	i = i + 1
	text:setText("自加 i = " .. i)
end)
