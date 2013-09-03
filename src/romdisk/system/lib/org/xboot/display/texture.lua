---
-- The 'texture' class lets you create a new texture object
-- to load from an image file and display in screen.
-- 
-- @module texture
local M = class()

---
-- Creates a new texture object.
-- 
-- @function [parent=#texture] new
-- @param filename (string) The name of the texture file to be loaded.
-- @return #texture 
function M:init(filename)
	if filename then
		self.surface = buildin_cairo.image_surface_create_from_png(filename);
	end
end

--- 
-- Returns the width of the texture in pixels.
-- 
-- @function [parent=#texture] get_width
-- @param self
-- @return The width of the texture in pixels.
function M:get_width()
	return self.surface:get_width()
end

---
-- Returns the height of the texture in pixels.
-- 
-- @function [parent=#texture] get_height
-- @param self
-- @return The height of the texture in pixels.
function M:get_height()
	return self.surface:get_height()
end

---
-- Returns the surface of the texture.
-- 
-- @function [parent=#texture] get_surface
-- @param self
-- @return The surface of the texture.
function M:get_surface()
	return self.surface
end

---
-- Creates a new texture object specifies a texture and a rectangular region in it.
-- 
-- @function [parent=#texture] region
-- @param self
-- @param x (number) left coordinate of the region
-- @param y (number) top coordinate of the region
-- @param width (number) width of the region
-- @param height (number) height of the region
-- @return #texture 
function M:region(x, y, width, height)
	x = x or 0
	y = y or 0
	width = width or self.get_width()
	height = height or self.get_height()

	local cs = buildin_cairo.surface_create_similar(self.surface, buildin_cairo.CONTENT_COLOR_ALPHA, width, height);
	local cr = buildin_cairo.create(cs)
	cr:set_source_surface(self.surface, -x, -y)
	cr:paint()
	local tex = M:new()
	tex.surface = cs
	return tex
end

return M
