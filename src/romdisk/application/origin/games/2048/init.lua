local Game = require "games.2048.Game"
local Pattern = Pattern

local M = Class(DisplayObject)

function M:init(w, h)
	self.super:init(w, h)

	local assets = assets
	self:addChild(DisplayShape.new(w, h)
		:setSource(Pattern.image(assets:loadImage("games/2048/images/bg.png")):setExtend("repeat"))
		:paint())
	self:addChild(Game.new(w, h):setLayoutEnable(true))
	self:setLayoutDirection("row"):setLayoutJustify("center"):setLayoutAlign("center")
end

return M
