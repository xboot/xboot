local ball = require("ball")

local background = display_image:new("background.png")
runtime:add_child(background)

-- create ball sprites
local ball1 = ball:new("ball1.png")
local ball2 = ball:new("ball2.png")
local ball3 = ball:new("ball3.png")
local ball4 = ball:new("ball4.png")
local ball5 = ball:new("ball5.png")

-- and add ball sprites to the runtime
runtime:add_child(ball1)
runtime:add_child(ball2)
runtime:add_child(ball3)
runtime:add_child(ball4)
runtime:add_child(ball5)
