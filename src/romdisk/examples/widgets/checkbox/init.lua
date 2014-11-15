local M = Class(DisplayObject)

function M:init()
	self.super:init()

	local w, h = application:getScreenSize()
	self:addChild(DisplayShape.new(w, h):setSourceColor(0.654, 1, 0):paint())

	local checkbox = Widget.CheckBox.new({x = 100, y = 100})
		:addEventListener("Change", function(d, e) print("CheckBox changed:", e.info.checked) end)
	self:addChild(checkbox)
end

return M
