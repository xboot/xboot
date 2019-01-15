local Game = require "games.2048.Game"
local Dobject = Dobject
local Pattern = Pattern

local M = Class(DisplayObject)

function M:init(w, h)
	self.super:init(w, h)

	local assets = assets
	self:addChild(DisplayShape.new(w, h)
		:setSource(Pattern.image(assets:loadImage("games/2048/images/bg.png")):setExtend(Pattern.EXTEND_REPEAT))
		:paint())
	self:setSize(w, h)
	self:addChild(Game.new(w, h):setAlignment(Dobject.ALIGN_CENTER))
	self:layout()
end

function M:setSize(width, height)
	self.super:setSize(width, height)
	return self
end

return M
