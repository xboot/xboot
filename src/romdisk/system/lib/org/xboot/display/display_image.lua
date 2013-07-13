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
	
	self.__image = buildin_cairo.image_surface_create_from_png(file);
	self.__w = self.__image:get_width()
	self.__h = self.__image:get_height()
	self.__x = x or 0
	self.__y = y or 0
	self.__visible = true
end

function M:render(cr)
	if not self.__visible then	return end

	cr:save()
	cr:set_source_surface(self.__image, self.__x, self.__y)
	cr:paint()
	cr:restore()

	local children = self:get_children()
	for i, v in ipairs(children) do v:render(cr) end
end

return M
