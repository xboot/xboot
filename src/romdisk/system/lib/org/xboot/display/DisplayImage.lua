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
-- @return #DisplayImage
function M:init(t, x, y)
	DisplayObject.init(self)

	self.x = x or 0
	self.y = y or 0

	self.texture = t
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
function M:__draw(cr)
	cr:save()

	if self.__scale or self.__rotate then
		local tx = self.x + self.anchorx
		local ty = self.y + self.anchory

		cr:translate(tx, ty)

		if self.__scale then
			cr:scale(self.scalex, self.scaley)
		end

		if self.__rotate then
			cr:rotate(self.rotation)
		end

		cr:translate(-tx, -ty)
	end

	cr:set_source_surface(self.texture:getSurface(), self.x, self.y)

	if self.__alpha then
		cr:paint_with_alpha(self.alpha)
	else
		cr:paint()
	end

	cr:restore()
end

return M
