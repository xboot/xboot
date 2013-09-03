local background = DisplayImage:new(Texture:new("background.png"))
runtime:addChild(background)

local t = Texture:new("font.png")
--local img = DisplayImage:new(t, 100, 100)
--runtime:addChild(img)

local t2 = t:region(50, 50, 150, 150)
local img2 = DisplayImage:new(t2, 50, 50)
runtime:addChild(img2)

local t1 = t2:region(0, 0, 50, 50)
local img1 = DisplayImage:new(t1, 200, 200)
runtime:addChild(img1)
