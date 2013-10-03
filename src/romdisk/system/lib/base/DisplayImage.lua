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
	self.super:init()

	self:setPosition(x or 0, y or 0)
	self:setTexture(texture)
end

---
-- Attach texture to display image.
--
-- @function [parent=#DisplayImage] setTexture
-- @param self
-- @param texture (Texture) The texture object
function M:setTexture(texture)
	self.texture = texture
end

---
-- Get texture of display image.
--
-- @function [parent=#DisplayImage] getTexture
-- @param self
-- @return The texture object of display image.
function M:getTexture()
	return self.texture
end

---
-- Returns the width and height of the display image in pixels. (subclasses method)
--
-- @function [parent=#DisplayImage] __size
-- @param self
-- @return The width and height of the display image.
function M:__size()
	local r = self:__bounds()
	return r.w, r.h
end

---
-- Returns a original table of rectangle (x, y, w and h) that encloses
-- the display shape in pixels. (subclasses method)
--
-- @function [parent=#DisplayImage] __bounds
-- @param self
-- @return table has 4 values as x, y, w and h of bounds
function M:__bounds()
	if self.texture then
		return self.texture:bounds()
	else
		return {x = 0, y = 0, w = 0, h = 0}
	end
end

---
-- Draw display image to the screen. (subclasses method)
--
-- @function [parent=#DisplayImage] __draw
-- @param self
-- @param display (Display) The context of the screen.
function M:__draw(display)
	if self.texture then
		display:drawTexture(self.texture, self:getTransformMatrix(), self:getAlpha())
	end
end

return M
