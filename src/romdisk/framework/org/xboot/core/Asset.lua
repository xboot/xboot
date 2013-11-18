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
	self.theme = {}
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

function M:loadDisplay(image)
	if type(image) == "string" then
		if string.lower(string.sub(image, -6)) == ".9.png" then
			return DisplayNinePatch.new(self:loadTexture(image))
		elseif string.lower(string.sub(image, -4)) == ".png" then
			return DisplayImage.new(self:loadTexture(image))
		end
	else
		return image
	end
end

function M:loadTheme(name)
	local name = name or "default"

	if not self.theme[name] then
		self.theme[name] = require("assets.themes." .. name)
	end

	return self.theme[name]
end

return M
