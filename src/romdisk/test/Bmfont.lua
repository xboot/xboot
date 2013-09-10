local M = Class(DisplayObject)

function M:init(fnt)
	DisplayObject.init(self)

	self.font = require(fnt)
	self.texture = Texture:new(self.font.texture.file)
	self.caches = {}
end

function M:setText(text)
	if text == nil then
		return
	end

	local x = 0
	local y = 0

	for c in string.gmatch(text, "[%z\1-\127\194-\244][\128-\191]*") do
		if not self.caches[c] then
			for i, v in ipairs(self.font.chars) do
				if v.char == c then
					self.caches[c] = v
					self.caches[c].texture = self.texture:region(v.x, v.y, v.w, v.h)
				end
			end
		end

		local char = self.caches[c]
		
		if char then
			local bitmap = DisplayImage:new(char.texture, self.x + char.ox + x, self.y - char.oy + y)
			self:addChild(bitmap)
			x = x + char.width
		end
	end
end

return M
