---
-- The 'Assets' class provides facilities to load and cache different
-- type of resources.
--
-- @module Assets
local M = Class()

---
-- Creates a new 'Assets' for cache different type of resources.
--
-- @function [parent=#Assets] new
-- @return New 'Assets' object.
function M:init()
	self.textures = {}
	self.ninepatches = {}
	self.fonts = {}
	self.themes = {}
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

function M:loadNinepatch(filename)
	if not filename then
		return nil
	end

	if not self.ninepatches[filename] then
		self.ninepatches[filename] = Ninepatch.new(filename)
	end

	return self.ninepatches[filename]
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
			return DisplayNinepatch.new(self:loadNinepatch(image))
		elseif string.lower(string.sub(image, -4)) == ".png" then
			return DisplayImage.new(self:loadTexture(image))
		end
	else
		return image
	end
end

function M:loadTheme(name)
	local name = name or "default"

	if not self.themes[name] then
		self.themes[name] = require("assets.themes." .. name)
	end

	return self.themes[name]
end

return M
