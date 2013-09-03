---
-- 
-- @module texture
local M = class()

function M:init(filename)
	if filename then
		self.surface = buildin_cairo.image_surface_create_from_png(filename);
	end
end

function M:get_width()
	return self.surface:get_width()
end

function M:get_height()
	return self.surface:get_height()
end

function M:get_surface()
	return self.surface
end

function M:region(x, y, width, height)
	local cs = buildin_cairo.surface_create_similar(self.surface, buildin_cairo.CONTENT_COLOR_ALPHA, width, height);
	local cr = buildin_cairo.create(cs)
	cr:set_source_surface(self.surface, -x, -y)
	cr:paint()
	local tex = M:new()
	tex.surface = cs
	return tex
end

return M
