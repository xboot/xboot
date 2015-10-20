---
-- The 'DisplayImageMask' class is used to display texture related objects that can
-- be placed on the screen.
--
-- @module DisplayImageMask
local M = class(DisplayObject)

---
-- Creates a new object of display image.
--
-- @function [parent=#DisplayImageMask] new
-- @param texture (Texture) The texture object
-- @param pattern (Pattern) The pattern for mask.
-- @return #DisplayImageMask
function M:init(texture, pattern)
	self.super:init()
	self:setTexture(texture)
	self:setPattern(pattern or Pattern.color(1, 1, 1, 0))
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
		self:setSize(w, h)
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
-- @function [parent=#DisplayText] setPattern
-- @param self
-- @param pattern (#Pattern) The new pattern for mask.
function M:setPattern(pattern)
	if pattern then
		self.pattern = pattern
	end
	return self
end

---
-- Gets the mask pattern.
-- 
-- @function [parent=#DisplayText] getPattern
-- @param self
-- @return pattern (#Pattern) The new pattern for mask.
function M:getPattern()
	return self.pattern
end

---
-- Draw display image with mask to the screen. (subclasses method)
--
-- @function [parent=#DisplayImageMask] __draw
-- @param self
-- @param display (Display) The context of the screen.
function M:__draw(display)
	self:updateTransformMatrix()
	display:drawTextureMask(self.object, self.texture, self.pattern)
end

return M
