local Animation = require("Animation")

local background = DisplayImage.new(Texture.new("background.png"))
stage:addChild(background)

local boy = asset:loadTexture("boy.png")
local boylist = {
	DisplayImage.new(boy:region(72 * 0, 0, 72, 72)),
	DisplayImage.new(boy:region(72 * 1, 0, 72, 72)),
	DisplayImage.new(boy:region(72 * 2, 0, 72, 72)),
	DisplayImage.new(boy:region(72 * 3, 0, 72, 72)),
	DisplayImage.new(boy:region(72 * 4, 0, 72, 72)),
	DisplayImage.new(boy:region(72 * 5, 0, 72, 72)),
	DisplayImage.new(boy:region(72 * 6, 0, 72, 72)),
	DisplayImage.new(boy:region(72 * 7, 0, 72, 72)),
	DisplayImage.new(boy:region(72 * 8, 0, 72, 72)),
	DisplayImage.new(boy:region(72 * 9, 0, 72, 72)),
	DisplayImage.new(boy:region(72 * 10, 0, 72, 72)),
}

local anmi = Animation.new(boylist, 100 ,100)
anmi:setAnchor(0.5, 0.5)
stage:addChild(anmi)

local font = Font.new("assets/fonts/Roboto-Regular.ttf")
local text = DisplayText.new(font)
text:setPosition(100,100)
text:setScale(100, 100)
stage:addChild(text)

local s = DisplayShape.new(100, 100)
stage:addChild(s)

s.shape:setSourceColor(1, 0.6, 0, 1)
s.shape:rectangle(0, 0, 100, 100)
s.shape:fill()
s.shape:setSourceColor(0, 0.6, 0.9, 1)
s.shape:rectangle(50, 50, 50, 50)
s.shape:stroke()

s:setPosition(150, 150)
s:setAnchor(0.5, 0.5)
s:setRotation(45)
--s:setScale(80, 80)

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

timermanager:addTimer(Timer.new(1, 0, function(t, e)
	anmi:rotate(10)
	--text:rotate(5)
	text:animate({rotation = text:getRotation() + 20}, 2/3, "bounce-out")
	s:rotate(6)
	
	i = i + 1
	text:setText("add i = " .. i)
end))






-- Create the btn
local btn = Widget.Button.new()
btn:setPosition(300, 100)
stage:addChild(btn)

-- Register to "btn" event
btn:addEventListener("Click", function(d, e)
	print("Button down")
end, btn)
