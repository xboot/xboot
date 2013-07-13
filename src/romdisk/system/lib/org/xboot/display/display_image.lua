local class = require("org.xboot.lang.class")
local event = require("org.xboot.event.event")
local event_dispatcher = require("org.xboot.event.event_dispatcher")
local display_object = require("org.xboot.display.display_object")
local buildin_cairo = require("org.xboot.buildin.cairo")

---
-- 
-- @module display_image
local M = class(display_object)

function M:init(file, x, y)
	display_object.init(self)
	
	self.image = buildin_cairo.image_surface_create_from_png(file);
	self.w = self.image:get_width()
	self.h = self.image:get_height()
	self.x = x or 0
	self.y = y or 0
	self.visible = true
end

function M:render(cr)
	if not self.visible then	return end

	cr:save()
	cr:translate(self.x, self.y)
	cr:scale(self.xscale, self.yscale)
	cr:rotate(self.rotation)
	cr:set_source_surface(self.image, 0, 0)
	cr:paint_with_alpha(self.alpha)
	cr:restore()

	local children = self:get_children()
	for i, v in ipairs(children) do v:render(cr) end
end

return M
