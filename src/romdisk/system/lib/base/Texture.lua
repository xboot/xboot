---
-- The 'Texture' class lets you create a new texture object
-- to load from an image file or others.
--
-- @module Texture
local M = Class()

---
-- Creates a new texture object.
--
-- @function [parent=#Texture] new
-- @param data (string) The name of the image file to be loaded.
-- @param data (userdata) The surface of the texture.
-- @return #Texture
function M:init(data)
	if type(data) == "string" then
		self.surface = buildin_cairo.image_surface_create_from_png(data);
	elseif type(data) == "userdata" then
		self.surface = data
	else
		self.surface = nil
	end
end

---
-- Returns the width of the texture in pixels.
--
-- @function [parent=#Texture] getWidth
-- @param self
-- @return The width of the texture in pixels.
function M:getWidth()
	if self.surface then
		return self.surface:get_width()
	end
	return 0
end

---
-- Returns the height of the texture in pixels.
--
-- @function [parent=#Texture] getHeight
-- @param self
-- @return The height of the texture in pixels.
function M:getHeight()
	if self.surface then
		return self.surface:get_height()
	end
	return 0
end

---
-- Sets the surface of the texture.
--
-- @function [parent=#Texture] setSurface
-- @param self
-- @param surface (userdata) The surface of the texture
function M:setSurface(surface)
	self.surface = surface or self.surface
end

---
-- Returns the surface of the texture.
--
-- @function [parent=#Texture] getSurface
-- @param self
-- @return The surface of the texture.
function M:getSurface()
	return self.surface
end

---
-- Creates a new texture object specifies a texture and a rectangular region in it.
--
-- @function [parent=#Texture] region
-- @param self
-- @param x (number) left coordinate of the region
-- @param y (number) top coordinate of the region
-- @param width (number) width of the region
-- @param height (number) height of the region
-- @return #Texture
function M:region(x, y, width, height)
	x = x or 0
	y = y or 0
	width = width or self.getWidth()
	height = height or self.getHeight()

	local cs = buildin_cairo.surface_create_similar(self.surface, buildin_cairo.CONTENT_COLOR_ALPHA, width, height);
	local cr = buildin_cairo.create(cs)
	cr:set_source_surface(self.surface, -x, -y)
	cr:paint()

	return M:new(cs)
end

return M
