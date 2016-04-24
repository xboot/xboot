local M = Class(DisplayObject)

function M:init(w, h)
	self.super:init()

	local assets = assets

	self:addChild(DisplayShape.new(w, h)
		:setSource(Pattern.texture(assets:loadTexture("widgets/radiobutton/bg.png")):setExtend(Pattern.EXTEND_REPEAT))
		:paint())

	local radiobutton = Widget.RadioButton.new({x = 100, y = 100})
		:addEventListener("Change", function(d, e) print("RadioButton changed:", e.checked) end)
	self:addChild(radiobutton)
end

return M
