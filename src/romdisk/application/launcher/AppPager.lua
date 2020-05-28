local AppItem = require "AppItem"

local M = Class(DisplayPager)

function M:init(width, height)
	self.super:init(width, height, false)
	self._font = "roboto"
	self._size = 24
	self._color = Color.new({255, 255, 255, 255})
	self:reload()
end

function M:setWidth(width)
	return self
end

function M:setHeight(height)
	return self
end

function M:setSize(width, height)
	return self
end

function M:reload()
	local pw, ph = self:getSize()
	local iw, ih = 128, 128
	local col = (pw * 2 - iw) // (iw * 3)
	local row = (ph * 2 - ih) // (ih * 3)
	local num = col * row
	local ox = (pw - col * iw) // (col + 1)
	local oy = (ph - row * ih) // (row + 1)

	local launcher = Application.new()
	local pages = {}
	local c = 0
	self:clear()

	for k, v in pairs(Application.list()) do
		if launcher:getPath() ~= k then
			local index = c // num + 1
			if not pages[index] then
				pages[index] = DisplayObject.new(pw, ph)
				self:addPage(pages[index])
			end

			local n = c % num
			local y = n // col
			local x = (n - y * col) % col
			local ix = (iw + ox) * x + ox
			local iy = (ih + oy) * y + oy
			local item = AppItem.new(v)
				:setPosition(ix, iy)
				:addEventListener("click", function(d, e)
					local path = d:getPath()
			  		for k, v in pairs(Window.list()) do
						if k == path then
			 				v:toFront()
			 				return
				        end
				    end
					d:execute()
					for k, v in pairs(Window.list()) do
						if k == path then
							v:toFront()
							break
				        end
				    end
				end)
			pages[index]:addChild(item)

			local label = DisplayText.new(v:getName(), self._color, 0, self._font, self._size)
			local w, h = label:getSize()
			label:setPosition((iw - w) / 2 + ix, iy + ih + 4)
			pages[index]:addChild(label)

			c = c + 1
		end
	end
end

return M
