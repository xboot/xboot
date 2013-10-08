---
-- The 'DisplayShape' class is used to display shape related objects that can
-- be placed on the screen.
--
-- @module DisplayShape
local M = Class(DisplayObject)

---
-- Creates a new object of display shape.
--
-- @function [parent=#DisplayShape] new
-- @param width (optional) The width of drawing area in pixels.
-- @param height (optional) The height of drawing area in pixels.
-- @return #DisplayShape
function M:init(width, height)
	self.super:init()

	self.shape = Shape.new(width or 1, height or 1)
	local w, h = self.shape:size()
	self:setSize(w, h)
end

---
-- Draw display shape to the screen. (subclasses method)
--
-- @function [parent=#DisplayShape] __draw
-- @param self
-- @param display (Display) The context of the screen.
function M:__draw(display)
	if self.shape then
		display:drawShape(self.shape, self:getTransformMatrix(), self:getAlpha())
	end
end

return M
