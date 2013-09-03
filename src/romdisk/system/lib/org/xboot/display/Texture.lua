---
-- The 'Texture' class lets you create a new texture object
-- to load from an image file and display in screen.
-- 
-- @module Texture
local M = Class()

---
-- Creates a new texture object.
-- 
-- @function [parent=#Texture] new
-- @param filename (string) The name of the texture file to be loaded.
-- @return #Texture 
function M:init(filename)
	if filename then
		self.surface = buildin_cairo.image_surface_create_from_png(filename);
	end
end

--- 
-- Returns the width of the texture in pixels.
-- 
-- @function [parent=#Texture] getWidth
-- @param self
-- @return The width of the texture in pixels.
function M:getWidth()
	return self.surface:get_width()
end

---
-- Returns the height of the texture in pixels.
-- 
-- @function [parent=#Texture] getHeight
-- @param self
-- @return The height of the texture in pixels.
function M:getHeight()
	return self.surface:get_height()
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
	local tex = M:new()
	tex.surface = cs
	return tex
end

return M
