local M = Class(DisplayObject)

function M:init(w, h)
	self.super:init()

	local assets = assets

	self:addChild(DisplayShape.new(w, h)
		:setSource(Pattern.texture(assets:loadTexture("widgets/checkbox/bg.png")):setExtend(Pattern.EXTEND_REPEAT))
		:paint())

	local checkbox = Widget.CheckBox.new({x = 100, y = 100})
		:addEventListener("Change", function(d, e) print("CheckBox changed:", e.checked) end)
	self:addChild(checkbox)
end

return M
