---
-- 
-- @module display_image
local M = class(display_object)

function M:init(t, x, y)
	display_object.init(self)

	self.x = x or 0
	self.y = y or 0

	self.texture = t
	self.width = self.texture:get_width()
	self.height = self.texture:get_height()
end

function M:update(cairo)
	cairo:save()

	if self.__scale or self.__rotate then
		local tx = self.x + self.anchorx
		local ty = self.y + self.anchory

		cairo:translate(tx, ty)

		if self.__scale then
			cairo:scale(self.scalex, self.scaley)
		end

		if self.__rotate then
			cairo:rotate(self.rotation)
		end

		cairo:translate(-tx, -ty)
	end

	cairo:set_source_surface(self.texture:get_surface(), self.x, self.y)

	if self.__alpha then
		cairo:paint_with_alpha(self.alpha)
	else
		cairo:paint()
	end

	cairo:restore()
end

return M
