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
	
	self.x = x or 0
	self.y = y or 0
	self.surface = buildin_cairo.image_surface_create_from_png(file);
	self.width = self.surface:get_width()
	self.height = self.surface:get_height()
	self.xorigin = self.width / 2
	self.yorigin = self.height / 2
end

function M:update(cairo)
	cairo:save()
	cairo:translate(self.xorigin + self.x, self.yorigin + self.y)
	cairo:scale(self.xscale, self.yscale)
	cairo:rotate(self.rotation)
	cairo:translate(-self.xorigin, -self.yorigin)
	cairo:set_source_surface(self.surface, 0, 0)
	cairo:paint_with_alpha(self.alpha)
	cairo:restore()
end

return M
