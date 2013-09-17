---
-- The 'DisplayImage' class is used to display texture related objects that can
-- be placed on the screen.
-- 
-- @module DisplayImage
local M = Class(DisplayObject)

---
-- Creates a new object of display image.
--
-- @function [parent=#DisplayImage] new
-- @param texture (Texture) The texture object
-- @param x (optional) The x coordinate of the display image.
-- @param y (optional) The y coordinate of the display image.
-- @return #DisplayImage
function M:init(texture, x, y)
	DisplayObject.init(self)

	self:setPosition(x or 0, y or 0)
	self.texture = texture
end

---
-- Returns the width and height of the display image in pixels. (subclasses method)
-- 
-- @function [parent=#DisplayImage] __size
-- @param self
-- @return The width and height of the display image.
function M:__size()
	return self.texture:getWidth(), self.texture:getHeight()
end

---
-- Draw display image to the screen. (subclasses method)
-- 
-- @function [parent=#DisplayImage] __draw
-- @param self
function M:__draw(display)
	display:draw(self.texture, self:getTransformMatrix(), self:getAlpha())
end

return M
