local DisplayObject = DisplayObject
local DisplayImage = DisplayImage
local Image = Image
local Xfs = Xfs

local M = Class(DisplayObject)

local function startsWith(str1, str2)
	return str1:sub(1, #str2) == str2
end

local function lineToTable(line)
	local result = {}
	for pair in line:gmatch("%a+=[-%d]+") do
		local key = pair:match("%a+")
		local value = pair:match("[-%d]+")
		result[key] = tonumber(value)
	end
	return result
end

function M:init(fontpng, fonttxt, text)
	self.super:init()

	self.image = Image.new(fontpng)
	self.chars = {}

	local file = Xfs.open(fonttxt, "r")
	for line in file:lines() do
		if startsWith(line, "char ") then
			local c = lineToTable(line)
			self.chars[c.id] = c
		end
	end
	file:close()

	self:setText(text)
end

function M:setText(text)
	self:removeChildren()

	local x = 0
	local y = 0
	for i = 1, #text do
		local c = self.chars[text:byte(i)]
		if c ~= nil then
			local t = self.image:clone(c.x, c.y, c.width, c.height)
			local bitmap = DisplayImage.new(t):setPosition(x + c.xoffset, y + c.yoffset)
			self:addChild(bitmap)
			x = x + c.xadvance
		end
	end
	self:setSize(x, 50)
	return self
end

return M
