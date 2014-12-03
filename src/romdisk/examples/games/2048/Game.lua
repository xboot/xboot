local M = Class(DisplayObject)

function M:init()
	self.super:init()
	
	local w, h = application:getScreenSize()
	local assets = application:getAssets()
	local bg = assets:loadDisplay("games/2048/gamebg.png")
	self:addChild(bg)
	local width, height = bg:getSize()
	self:setSize(width, height)
	
	self.cells = {}
	for i = 1, 4 do
		if not self.cells[i] then
			self.cells[i] = {}
		end
		for j = 1, 4 do
			self.cells[i][j] = {v = 0, o = nil}
		end
	end


--	self:test()
end

function M:setSize(width, height)
	self.super:setSize(width, height)
	return self
end

function M:insertTile(x, y, n)
	local tile

	self:removeTile(x, y)
	if n ~= 0 then
		local assets = application:getAssets()
		local o = assets:loadDisplay("games/2048/tile" .. n .. ".png")
			:setPosition((x - 1) * 70 + 55, (y - 1) * 70 + 171)
			:setAnchor(0.5, 0.5)
		self:addChild(o)
		tile = {v = n, o = o}
	else
		tile = {v = 0, o = nil}
	end
	
	self.cells[x][y] = tile
	return tile
end

function M:removeTile(x, y)
	self:removeChild(self.cells[x][y].o)
	self.cells[x][y] = {v = 0, o = nil}
end

function M:randomTile()
	local cells = {}
	for i = 1, 4 do
		for j = 1,4 do
			if self.cells[i][j].v == 0 then
				table.insert(cells, {x = i, y = j})
			end
		end
	end
	if #cells > 0 then
		local c = cells[1 + math.floor(math.random() * #cells)]
		local tile = self:insertTile(c.x, c.y, 2 + 2 * math.floor(math.random() / 0.9))
		tile.o:setScale(0.1, 0.1):animate({scalex = 1, scaley = 1}, 0.3, "outBounce")
	end
end

function M:isGameOver()
	for i=1, 4 do
		for j=1, 4 do
			if self.cells[i][j].v == 0 or
				(j + 1 <= 4 and self.cells[i][j].v == self.cells[i][j + 1].v) or
				(i + 1 <= 4 and self.cells[i][j].v == self.cells[i + 1][j].v) then
				return false
			end
		end
	end
	return true
end

function M:test()
	for i = 1, 4 do
		for j = 1, 4 do
			self:insertTile(i, j, 128)
		end
	end
end

return M
