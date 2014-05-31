---
-- The 'DisplayNinepatch' class is used to display nine patch related objects that can
-- be placed on the screen.
--
-- @module DisplayNinepatch
local M = Class(DisplayObject)

---
-- Creates a new object of display nine patch.
--
-- @function [parent=#DisplayNinepatch] new
-- @param ninepatch (Ninepatch) The ninepatch object
-- @param width (number) The width of drawing area in pixels.
-- @param height (number) The height of drawing area in pixels.
-- @return #DisplayNinepatch
function M:init(ninepatch, width, height)
	self.super:init()
	self:setNinepatch(ninepatch, width, height)
end

function M:setSize(width, height)
	self.super:setSize(width, height)
	if self.ninepatch then
		self.ninepatch:setSize(width, height)
	end
	return self
end

---
-- Attach ninepatch to display ninepatch.
--
-- @function [parent=#DisplayNinepatch] setNinepatch
-- @param self
-- @param ninepatch (Ninepatch) The ninepatch object
function M:setNinepatch(ninepatch, width, height)
	if ninepatch then
		local w, h = ninepatch:getSize()
		self.ninepatch = ninepatch
		self:setSize(width or w, height or h)
	end
	return self
end

---
-- Get ninepatch of display ninepatch.
--
-- @function [parent=#DisplayNinepatch] getNinepatch
-- @param self
-- @return The ninepatch object
function M:getNinepatch()
	return self.ninepatch
end

---
-- Draw display ninepatch to the screen. (subclasses method)
--
-- @function [parent=#DisplayNinepatch] __draw
-- @param self
-- @param display (Display) The context of the screen.
function M:__draw(display)
	display:drawNinepatch(self.ninepatch, self:getTransformMatrix(), self:getAlpha())
end

return M
