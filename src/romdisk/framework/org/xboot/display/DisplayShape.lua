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
-- @param width (number) The width of drawing area in pixels.
-- @param height (number) The height of drawing area in pixels.
-- @param x (optional) The x coordinate of the display shape.
-- @param y (optional) The y coordinate of the display shape. 
-- @return #DisplayShape
function M:init(width, height, x, y)
	self.super:init()

	self:setShape(Shape.new(width, height))
	self:setPosition(x or 0, y or 0)
end

---
-- Attach shape to display shape.
--
-- @function [parent=#DisplayShape] setShape
-- @param self
-- @param shape (Shape) The shape object
function M:setShape(shape)
	if shape then
		local w, h = shape:size()
		self.shape = shape
		self:setInnerSize(w, h)
	end
end

---
-- Get shape of display shape.
--
-- @function [parent=#DisplayShape] getShape
-- @param self
-- @return The shape object of display shape.
function M:getShape()
	return self.shape
end

function M:save()
	self.shape:save()
end

function M:restore()
	self.shape:restore()
end

---
-- Draw display shape to the screen. (subclasses method)
--
-- @function [parent=#DisplayShape] __draw
-- @param self
-- @param display (Display) The context of the screen.
function M:__draw(display)
	display:drawShape(self.shape, self:getTransformMatrix(), self:getAlpha())
end

return M
