local Ball = require("Ball")

local background = DisplayImage.new(Texture.new("background.png"))
runtime:addChild(background)

-- create ball sprites
local ball1 = Ball.new(Texture.new("ball1.png"))
local ball2 = Ball.new(Texture.new("ball2.png"))
local ball3 = Ball.new(Texture.new("ball3.png"))
local ball4 = Ball.new(Texture.new("ball4.png"))
local ball5 = Ball.new(Texture.new("ball5.png"))

-- and add ball sprites to the runtime
runtime:addChild(ball1)
runtime:addChild(ball2)
runtime:addChild(ball3)
runtime:addChild(ball4)
runtime:addChild(ball5)
