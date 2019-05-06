local Ball = require "graphics.balls.Ball"
local Pattern = Pattern

local M = Class(DisplayObject)

function M:init(w, h)
	self.super:init(w, h)

	self:addChild(DisplayShape.new(w, h)
		:setSource(Pattern.image(assets:loadImage("graphics/balls/bg.png")):setExtend("repeat"))
		:paint())

	self:addChild(Ball.new("graphics/balls/ball1.png", w, h))
	self:addChild(Ball.new("graphics/balls/ball2.png", w, h))
	self:addChild(Ball.new("graphics/balls/ball3.png", w, h))
	self:addChild(Ball.new("graphics/balls/ball4.png", w, h))
	self:addChild(Ball.new("graphics/balls/ball5.png", w, h))
end

return M
