local M = class(DisplayObject)

function M:init()
	self.super:init()

	local w, h = application:getScreenSize()
	local assets = application:getAssets()

	self:addChild(DisplayShape.new(w, h)
		:setSource(Pattern.texture(assets:loadTexture("widgets/checkbox/bg.png")):setExtend(Pattern.EXTEND_REPEAT))
		:paint())

	local checkbox = Widget.CheckBox.new({x = 100, y = 100})
		:addEventListener("Change", function(d, e) print("CheckBox changed:", e.info.checked) end)
	self:addChild(checkbox)
end

return M
