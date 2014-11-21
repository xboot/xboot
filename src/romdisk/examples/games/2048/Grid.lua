local Tile = require("games.2048.Tile")

local M = Class(DisplayObject)

function M:init()
	self.super:init()
	
	self.cells = {}
	self:clear()
end

function M:clear()
	for i = 1, 4 do
		if not self.cells[i] then
			self.cells[i] = {}
		end
		for j = 1, 4 do
			self.cells[i][j] = 0
		end
	end
end

function M:insert(x, y, n)
	self.cells[x][y] = n
end

function M:remove(x, y)
	self.cells[x][y] = 0
end

function M:randomAvailableCell()
	local freecells = {}
	
	for i = 1, 4 do
		for j = 1,4 do
			if self.cells[i][j] == 0 then
				table.insert(freecells, {x = i, y = j})
			end
		end
	end
	if #freecells > 0 then
		return freecells[1 + math.floor(math.random() * #freecells)]
	end
end

return M
