local Button = require "xboot.widget.Button"
local Pattern = Pattern

local M = Class(DisplayObject)

function M:init(w, h)
	self.super:init(w, h)

	local assets = assets

	self:addChild(DisplayShape.new(w, h)
		:setSource(Pattern.image(assets:loadImage("widgets/button/bg.png")):setExtend("repeat"))
		:paint())

	local button = Button.new({x = 100, y = 100, width = 100, height = 50})
		:addEventListener("press", function(d, e) print("Button [Press]") end)
		:addEventListener("release", function(d, e) print("Button [Release]") end)
		:addEventListener("click", function(d, e) print("Button [Click]") end)
	self:addChild(button)
end

return M
