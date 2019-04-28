local RadioButton = require "xboot.widget.RadioButton"
local Pattern = Pattern

local M = Class(DisplayObject)

function M:init(w, h)
	self.super:init(w, h)

	local assets = assets

	self:addChild(DisplayShape.new(w, h)
		:setSource(Pattern.image(assets:loadImage("widgets/radiobutton/bg.png")):setExtend("repeat"))
		:paint())

	local radiobutton = RadioButton.new({x = 100, y = 100})
		:addEventListener("change", function(d, e) print("RadioButton changed:", e.checked) end)
	self:addChild(radiobutton)
end

return M
