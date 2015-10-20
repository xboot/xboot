local M = class(DisplayObject)

function M:init()
	self.super:init()

	local w, h = application:getScreenSize()
	local assets = application:getAssets()

	self:addChild(DisplayShape.new(w, h)
		:setSource(Pattern.texture(assets:loadTexture("widgets/radiobutton/bg.png")):setExtend(Pattern.EXTEND_REPEAT))
		:paint())

	local radiobutton = Widget.RadioButton.new({x = 100, y = 100})
		:addEventListener("Change", function(d, e) print("RadioButton changed:", e.info.checked) end)
	self:addChild(radiobutton)
end

return M
