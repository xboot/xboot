---
-- The 'Asset' class provides facilities to load and cache different
-- type of resources.
--
-- @module Asset
local M = Class()

---
-- Creates a new 'Asset' for cache different type of resources.
--
-- @function [parent=#Asset] new
-- @return New 'Asset' object.
function M:init()
	self.textures = {}
	self.fonts = {}
end

function M:loadTexture(filename)
	if not filename then
		return nil
	end

	if not self.textures[filename] then
		self.textures[filename] = Texture.new(filename)
	end

	return self.textures[filename]
end

function M:loadFont(family)
	if not family then
		return nil
	end

	if not self.fonts[family] then
		self.fonts[family] = Font.new(family)
	end

	return self.fonts[family]
end

return M
