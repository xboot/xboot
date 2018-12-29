local M = Class(DisplayObject)

local function startsWith(string1, string2)
	return string1:sub(1, #string2) == string2
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

function M:init(fntfile, imgfile, str)
	DisplayObject.init(self)

	self.texture = Texture.new(imgfile)
	self.chars = {}
	
	local file = io.open(fntfile, "r")
	for line in file:lines() do
		if startsWith(line, "char ") then
			local char = lineToTable(line)
			self.chars[char.id] = char
		end
	end
	io.close(file)

	self.str = str
	self:createCharacters()
end

function M:setText(str)
	if self.str ~= str then
		self.str = str
		self:createCharacters()
	end
end

function M:createCharacters()
	self:removeChildren()
	
	local x = 0
	local y = 0
	for i=1,#self.str do
		local char = self.chars[self.str:byte(i)]
		if char ~= nil then

			local t = self.texture:region(char.x, char.y, char.width, char.height)
			local bitmap = DisplayImage.new(t, self.x + x + char.xoffset, self.y + y + char.yoffset)

			self:addChild(bitmap)
			x = x + char.xadvance
		end
	end
end

return M
