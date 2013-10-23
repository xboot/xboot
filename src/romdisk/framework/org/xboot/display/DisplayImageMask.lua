---
-- The 'DisplayImageMask' class is used to display texture related objects that can
-- be placed on the screen.
--
-- @module DisplayImageMask
local M = Class(DisplayObject)

---
-- Creates a new object of display image.
--
-- @function [parent=#DisplayImageMask] new
-- @param texture (Texture) The texture object
-- @param parttern (Parttern) The parttern for mask.
-- @param x (optional) The x coordinate of the display image.
-- @param y (optional) The y coordinate of the display image.
-- @return #DisplayImageMask
function M:init(texture, parttern, x, y)
	self.super:init()

	self:setTexture(texture)
	self:setParttern(parttern)
	self:setPosition(x or 0, y or 0)
end

---
-- Attach texture to display image.
--
-- @function [parent=#DisplayImageMask] setTexture
-- @param self
-- @param texture (Texture) The texture object
function M:setTexture(texture)
	self.texture = texture
	if self.texture then
		local w, h = self.texture:size()
		self:setSize(w, h)
	else
		self.setSize(0, 0)
	end
end

---
-- Get texture of display image.
--
-- @function [parent=#DisplayImageMask] getTexture
-- @param self
-- @return The texture object of display image.
function M:getTexture()
	return self.texture
end

---
-- Sets the display image's mask.
-- 
-- @function [parent=#DisplayText] setParttern
-- @param self
-- @param parttern (#Parttern) The new parttern for mask.
function M:setParttern(parttern)
	self.parttern = parttern
end

---
-- Gets the mask parttern.
-- 
-- @function [parent=#DisplayText] getParttern
-- @param self
-- @return parttern (#Parttern) The new parttern for mask.
function M:getParttern()
	return self.parttern
end

---
-- Draw display image with mask to the screen. (subclasses method)
--
-- @function [parent=#DisplayImageMask] __draw
-- @param self
-- @param display (Display) The context of the screen.
function M:__draw(display)
	if self.texture and self.parttern then
		display:drawTextureMask(self.texture, self.parttern, self:getTransformMatrix())
	end
end

return M
