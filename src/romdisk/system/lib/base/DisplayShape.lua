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
end

---
-- Returns the width and height of the display shape in pixels. (subclasses method)
--
-- @function [parent=#DisplayShape] __size
-- @param self
-- @return The width and height of the display shape.
function M:__size()
	local r = self:__bounds()
	return r.w, r.h
end

---
-- Returns a original table of rectangle (x, y, w and h) that encloses
-- the display shape in pixels. (subclasses method)
--
-- @function [parent=#DisplayShape] __bounds
-- @param self
-- @return table has 4 values as x, y, w and h of bounds
function M:__bounds()
	if self.shape then
		return self.shape:bounds()
	else
		return {x = 0, y = 0, w = 0, h = 0}
	end
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
