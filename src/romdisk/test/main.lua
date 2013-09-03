local background = display_image:new(texture:new("background.png"))
runtime:add_child(background)

local t = texture:new("font.png")
--local img = display_image:new(t, 100, 100)
--runtime:add_child(img)

local t2 = t:region(50, 50, 150, 150)
local img2 = display_image:new(t2, 50, 50)
runtime:add_child(img2)

local t1 = t2:region(0, 0, 50, 50)
local img1 = display_image:new(t1, 200, 200)
runtime:add_child(img1)
