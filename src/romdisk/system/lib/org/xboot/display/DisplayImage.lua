---
-- 
-- @module DisplayImage
local M = Class(DisplayObject)

function M:init(t, x, y)
	DisplayObject.init(self)

	self.x = x or 0
	self.y = y or 0

	self.texture = t
end

function M:__size()
	return self.texture:getWidth(), self.texture:getHeight()
end

function M:__update(cr)
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
