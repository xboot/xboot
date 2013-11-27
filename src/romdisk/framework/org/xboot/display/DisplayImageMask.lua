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
-- @return #DisplayImageMask
function M:init(texture, parttern)
	self.super:init()
	self:setTexture(texture)
	self:setParttern(parttern or Parttern.color(1, 1, 1, 0))
end

---
-- Attach texture to display image.
--
-- @function [parent=#DisplayImageMask] setTexture
-- @param self
-- @param texture (Texture) The texture object
function M:setTexture(texture)
	if texture then
		local w, h = texture:size()
		self.texture = texture
		self:setInnerSize(w, h)
	end
	return self
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
	if parttern then
		self.parttern = parttern
	end
	return self
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
	display:drawTextureMask(self.texture, self.parttern, self:getTransformMatrix())
end

return M
