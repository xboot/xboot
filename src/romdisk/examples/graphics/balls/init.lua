local Ball = require("graphics.balls.Ball")

local M = class(DisplayObject)

function M:init()
	self.super:init()

	local w, h = application:getScreenSize()
	local assets = application:getAssets()

	self:addChild(DisplayShape.new(w, h)
		:setSource(Pattern.texture(assets:loadTexture("graphics/balls/bg.png")):setExtend(Pattern.EXTEND_REPEAT))
		:paint())

	self:addChild(Ball.new("graphics/balls/ball1.png"))
	self:addChild(Ball.new("graphics/balls/ball2.png"))
	self:addChild(Ball.new("graphics/balls/ball3.png"))
	self:addChild(Ball.new("graphics/balls/ball4.png"))
	self:addChild(Ball.new("graphics/balls/ball5.png"))
end

return M
