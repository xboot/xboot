---
-- The 'DisplayText' class is used to display text related objects that can
-- be placed on the screen.
--
-- @module DisplayText
local M = Class(DisplayObject)

---
-- Creates a new object of display text.
--
-- @function [parent=#DisplayText] new
-- @param font (Font) The font object
-- @param x (optional) The x coordinate of the display image.
-- @param y (optional) The y coordinate of the display image.
-- @return #DisplayText
function M:init(font, text, parttern)
	DisplayObject.init(self)

	self:setFont(font)
	self:setText(text)
	self:setParttern(parttern or Parttern.rgba(1, 1, 1))
end

function M:setFont(font)
	self.font = font
end

function M:getFont()
	return self.font
end

function M:setText(text)
	self.text = text
end

function M:getText()
	return self.text
end

function M:setParttern(parttern)
	self.parttern = parttern
end

function M:getParttern()
	return self.parttern
end

---
-- Returns the width and height of the display image in pixels. (subclasses method)
--
-- @function [parent=#DisplayText] __size
-- @param self
-- @return The width and height of the display image.
function M:__size()
	if self.texture then
		return self.texture:getWidth(), self.texture:getHeight()
	else
		return 10, 10
	end
end

---
-- Draw display image to the screen. (subclasses method)
--
-- @function [parent=#DisplayText] __draw
-- @param self
-- @param display (Display) The context of the screen.
function M:__draw(display)
	if self.text then
		display:drawText(self.font, self.text, self.parttern, self:getTransformMatrix())
	end
end

return M
