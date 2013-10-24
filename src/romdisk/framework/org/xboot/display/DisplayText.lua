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
-- @param font (Font) The font of the display text.
-- @param size (optional) Sets the current font matrix to a scale by a factor of size.
-- @param parttern (optional) The parttern of the display text.
-- @param text (optional) The content of display text that will be placed on the screen.
-- @return #DisplayText
function M:init(font, size, parttern, text)
	self.super:init()

	self:setFont(font)
	self:setFontSize(size or 10)
	self:setParttern(parttern or Parttern.color())
	self:setText(text or "")
end

---
-- Sets the display text's font.
-- 
-- @function [parent=#DisplayText] setFont
-- @param self
-- @param font (#Font) The new font of display text.
function M:setFont(font)
	if font then
		self.font = font
	end
end

---
-- Gets the display text's font.
-- 
-- @function [parent=#DisplayText] getFont
-- @param self
-- @return font (#Font) The font of the display text.
function M:getFont()
	return self.font
end

---
-- Sets the display text's font size.
-- 
-- @function [parent=#DisplayText] setFontSize
-- @param self
-- @param x (number) horizontal font size of the display text.
-- @param y (number) vertical font size of the display text.
function M:setFontSize(x, y)
	self:setScale(x, y or x)
end

---
-- Gets the display text's font size.
-- 
-- @function [parent=#DisplayText] getFontSize
-- @param self
-- @return The horizontal and vertical font size of the display text.
function M:getFontSize()
	return self:getScale()
end

---
-- Sets the display text's parttern.
-- 
-- @function [parent=#DisplayText] setParttern
-- @param self
-- @param parttern (#Parttern) The new parttern of display text.
function M:setParttern(parttern)
	if parttern then
		self.parttern = parttern
	end
end

---
-- Gets the display text's parttern.
-- 
-- @function [parent=#DisplayText] getParttern
-- @param self
-- @return parttern (#Parttern) The parttern of the display text.
function M:getParttern()
	return self.parttern
end

---
-- Sets the display text's content.
-- 
-- @function [parent=#DisplayText] setText
-- @param self
-- @param text (string) The new content of display text.
function M:setText(text)
	if text and self.font then
		local w, h = self.font:size(text)
		self.text = text
		self:setInnerSize(w, h)
	end
end

---
-- Gets the display text's content.
-- 
-- @function [parent=#DisplayText] setText
-- @param self
-- @return text (string) The content of the display text.
function M:getText()
	return self.text
end

---
-- Draw display text to the screen. (subclasses method)
--
-- @function [parent=#DisplayText] __draw
-- @param self
-- @param display (Display) The context of the screen.
function M:__draw(display)
	if self.font and self.text then
		display:drawText(self.font, self.text, self.parttern, self:getTransformMatrix())
	end
end

return M
