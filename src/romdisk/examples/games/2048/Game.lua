local M = Class(DisplayObject)

function M:init()
	self.super:init()
	
	local w, h = application:getScreenSize()
	local assets = application:getAssets()
	local bg = assets:loadDisplay("games/2048/gamebg.png")
	local width, height = bg:getSize()
	self:setSize(width, height)
	self:addChild(bg)

	local button = Widget.Button.new({x = 130, y = 90,
		imageNormal = "games/2048/restartNormal.png",
		imagePressed = "games/2048/restartPressed.png",
		imageDisabled = "games/2048/restartDisabled.png"})
		:addEventListener("Release", function(d, e) self:restart() end, self)
	self:addChild(button)

	self.cells = {}
	for y = 1, 4 do
		if not self.cells[y] then
			self.cells[y] = {}
		end
		for x = 1, 4 do
			self.cells[y][x] = {n = 0, o = nil}
		end
	end
	
	self:restart()
end

function M:setSize(width, height)
	self.super:setSize(width, height)
	return self
end

function M:addTile(x, y, n)
	local tile
	self:removeTile(x, y)
	if n ~= 0 then
		local assets = application:getAssets()
		local o = assets:loadDisplay("games/2048/tile" .. n .. ".png")
			:setPosition((x - 1) * 70 + 55, (y - 1) * 70 + 171)
			:setAnchor(0.5, 0.5)
		self:addChild(o)
		tile = {n = n, o = o}
	else
		tile = {n = 0, o = nil}
	end
	self.cells[y][x] = tile
	return tile
end

function M:removeTile(x, y)
	self:removeChild(self.cells[y][x].o)
	self.cells[y][x] = {n = 0, o = nil}
end

function M:moveTile(fx, fy, tx, ty)
	local tile = self.cells[fy][fx]
	if self.cells[ty][tx].n == 0 then
		self.cells[ty][tx] = tile
		self.cells[fy][fx] = {n = 0, o = nil}
		tile.o:animate({x = (tx - 1) * 70 + 55, y = (ty - 1) * 70 + 171}, 0.5, "outBounce")
	else
	end
end

function M:randomTile()
	local cells = {}
	for y = 1, 4 do
		for x = 1, 4 do
			if self.cells[y][x].n == 0 then
				table.insert(cells, {x = x, y = y})
			end
		end
	end
	if #cells > 0 then
		local c = cells[1 + math.floor(math.random() * #cells)]
		local tile = self:addTile(c.x, c.y, 2 + 2 * math.floor(math.random() / 0.9))
		tile.o:setScale(0.1, 0.1):animate({scalex = 1, scaley = 1}, 0.3, "outBounce")
	end
end

function M:isGameOver()
	for y = 1, 4 do
		for x = 1, 4 do
			if self.cells[y][x].n == 0 or
				(x + 1 <= 4 and self.cells[y][x].n == self.cells[y][x + 1].n) or
				(y + 1 <= 4 and self.cells[y][x].n == self.cells[y + 1][x].n) then
				return false
			end
		end
	end
	return true
end

function M:restart()
	for y = 1, 4 do
		for x = 1, 4 do
			self:removeTile(x, y)
		end
	end
	self:randomTile()
	self:randomTile()
end

function M:moveLeft()

	for y = 1, 4 do
		for x = 2, 4 do
			if self.cells[y][x].n ~= 0 then
				self:moveTile(x, y, 1, y)
			end
		end
	end


end


return M
