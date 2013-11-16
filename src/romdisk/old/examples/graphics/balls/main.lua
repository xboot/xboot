local Ball = require("Ball")

local background = DisplayImage.new(Texture.new("background.png"))
stage:addChild(background)

-- create ball sprites
local ball1 = Ball.new(Texture.new("ball1.png"))
local ball2 = Ball.new(Texture.new("ball2.png"))
local ball3 = Ball.new(Texture.new("ball3.png"))
local ball4 = Ball.new(Texture.new("ball4.png"))
local ball5 = Ball.new(Texture.new("ball5.png"))

-- and add ball sprites to the stage
stage:addChild(ball1)
stage:addChild(ball2)
stage:addChild(ball3)
stage:addChild(ball4)
stage:addChild(ball5)
