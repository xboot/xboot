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
	if self.texture then
		return self.texture:getWidth(), self.texture:getHeight()
	else
		return 0, 0
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
