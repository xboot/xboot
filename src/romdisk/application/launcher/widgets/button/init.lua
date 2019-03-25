local Button = require "xboot.widget.Button"
local Pattern = Pattern

local M = Class(DisplayObject)

function M:init(w, h)
	self.super:init()

	local assets = assets

	self:addChild(DisplayShape.new(w, h)
		:setSource(Pattern.image(assets:loadImage("widgets/button/bg.png")):setExtend(Pattern.EXTEND_REPEAT))
		:paint())

	local button = Button.new({x = 100, y = 100, width = 100, height = 50})
		:addEventListener("Press", function(d, e) print("Button [Press]") end)
		:addEventListener("Release", function(d, e) print("Button [Release]") end)
		:addEventListener("Click", function(d, e) print("Button [Click]") end)
	self:addChild(button)
end

return M
