local M = Class(DisplayObject)

function M:init()
	self.super:init()

	local w, h = application:getScreenSize()
	self:addChild(DisplayShape.new(w, h):setSourceColor(0.4, 0.647, 0.5):paint())

	local radiobutton = Widget.RadioButton.new({x = 100, y = 100})
		:addEventListener("Change", function(d, e) print("RadioButton changed:", e.info.checked) end)
	self:addChild(radiobutton)
end

return M
