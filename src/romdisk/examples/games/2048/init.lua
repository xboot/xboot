local Game = require("games.2048.Game")

local M = class(DisplayObject)

function M:init()
	self.super:init()
	
	local w, h = application:getScreenSize()
	local assets = application:getAssets()
	self:addChild(DisplayShape.new(w, h)
		:setSource(Pattern.texture(assets:loadTexture("games/2048/images/bg.png")):setExtend(Pattern.EXTEND_REPEAT))
		:paint())
	self:setSize(w, h)
	self:addChild(Game.new():setAlignment(Object.ALIGN_CENTER))
	self:layout()
end

function M:setSize(width, height)
	self.super:setSize(width, height)
	return self
end

return M
