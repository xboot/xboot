local Ball = require("graphics.balls.Ball")

local M = Class(DisplayObject)

function M:init()
	self.super:init()

	self:addChild(Ball.new("graphics/balls/ball1.png"))
	self:addChild(Ball.new("graphics/balls/ball2.png"))
	self:addChild(Ball.new("graphics/balls/ball3.png"))
	self:addChild(Ball.new("graphics/balls/ball4.png"))
	self:addChild(Ball.new("graphics/balls/ball5.png"))
end

return M
