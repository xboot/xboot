local Button = require "xboot.widget.Button"
local Tile = require("Tile")
local Score = require("Score")
local M = Class(DisplayObject)

function M:init()
	self.super:init()

	local background = assets:loadDisplay("assets/images/background.png")
	local width, height = background:getSize()
	self:setSize(width, height)
	self:addChild(background)

	local clearbutton = Button.new({x = 130, y = 90,
		imageNormal = "assets/images/clearNormal.png",
		imagePressed = "assets/images/clearPressed.png",
		imageDisabled = "assets/images/clearDisabled.png"})
		:addEventListener("Release", function(d, e) d:clear() end, self)
	self:addChild(clearbutton)

	local restartbutton = Button.new({x = 225, y = 90,
		imageNormal = "assets/images/restartNormal.png",
		imagePressed = "assets/images/restartPressed.png",
		imageDisabled = "assets/images/restartDisabled.png"})
		:addEventListener("Release", function(d, e) d:restart() end, self)
	self:addChild(restartbutton)

	self.score = Score.new(0):setPosition(140, 50)
	self.bestscore = Score.new(0):setPosition(235, 50)
	self:addChild(self.score)
	self:addChild(self.bestscore)

	self.cells = {}
	for y = 1, 4 do
		if not self.cells[y] then
			self.cells[y] = {}
		end
		for x = 1, 4 do
			self.cells[y][x] = {}
		end
	end
	
	self:restart()
	self:addEventListener(Event.KEY_DOWN, self.onKeyDown)
end

function M:setSize(width, height)
	self.super:setSize(width, height)
	return self
end

function M:getNumber(x, y)
	local n = 0
	for i, v in ipairs(self.cells[y][x]) do
		n = n + v:number()
	end
	return n
end

function M:addTile(x, y, n)
	local tile = Tile.new(x, y, n)
	table.insert(self.cells[y][x], tile)
	self:addChild(tile)
end

function M:removeTile(x, y)
	for i, v in ipairs(self.cells[y][x]) do
		self:removeChild(v)
	end
	self.cells[y][x] = {}
end

function M:moveTile(fx, fy, tx, ty)
	if tx ~= fx or ty ~= fy then
		for i, v in ipairs(self.cells[fy][fx]) do
			table.insert(self.cells[ty][tx], v)
			v:move(tx, ty)
		end
		self.cells[fy][fx] = {}
		return true
	end
	return false
end

function M:randomTile()
	local cells = {}
	for y = 1, 4 do
		for x = 1, 4 do
			if self:getNumber(x, y) == 0 then
				table.insert(cells, {x = x, y = y})
			end
		end
	end
	if #cells > 0 then
		local c = cells[1 + math.floor(math.random() * #cells)]
		self:addTile(c.x, c.y, 2 + 2 * math.floor(math.random() / 0.9))
	end
end

function M:mergeTile()
	for y = 1, 4 do
		for x = 1, 4 do
			if #self.cells[y][x] > 1 then
				local n = 0
				for i, v in ipairs(self.cells[y][x]) do
					n = n + v:number()
				end
				self:removeTile(x, y)
				self:addTile(x, y, n)

				self.score:set(self.score:get() + n)
				if self.bestscore:get() < self.score:get() then
					self.bestscore:set(self.score:get())
				end
			end
		end
	end
end

function M:clear()
  self.score:set(0)
  self.bestscore:set(0)
end

function M:restart()
	for y = 1, 4 do
		for x = 1, 4 do
			self:removeTile(x, y)
		end
	end
	self.score:set(0)
	self:randomTile()
	self:randomTile()
end

function M:isGameOver()
	for y = 1, 4 do
		for x = 1, 4 do
			if self:getNumber(x, y) == 0 or
				(x + 1 <= 4 and self:getNumber(x, y) == self:getNumber(x + 1, y)) or
				(y + 1 <= 4 and self:getNumber(x, y) == self:getNumber(x, y + 1)) then
				return false
			end
		end
	end
	return true
end

function M:moveLeft()
	local changed = false

	for y = 1, 4 do
		local line = {}
		for x = 1, 4 do
			line[x] = {fx = x, fy = y, tx = x, ty = y, n = self:getNumber(x, y)}
		end

		local n = 0
		local o = 0
		for x = 1, 4 do
			if line[x].n ~= 0 then
				if n == 0 then
					o = o + 1
					n = line[x].n
					line[x].tx = o
				elseif line[x].n == n then
					n = 0
					line[x].tx = o
				else
					o = o + 1
					n = line[x].n
					line[x].tx = o
				end
			end
		end
		
		for x = 1, 4 do
			if self:moveTile(line[x].fx, line[x].fy, line[x].tx, line[x].ty) then
				changed = true
			end
		end
	end

	self:mergeTile()
	return changed
end

function M:moveRight()
	local changed = false

	for y = 1, 4 do
		local line = {}
		for x = 1, 4 do
			line[x] = {fx = x, fy = y, tx = x, ty = y, n = self:getNumber(x, y)}
		end

		local n = 0
		local o = 5
		for x = 4, 1, -1 do
			if line[x].n ~= 0 then
				if n == 0 then
					o = o - 1
					n = line[x].n
					line[x].tx = o
				elseif line[x].n == n then
					n = 0
					line[x].tx = o
				else
					o = o - 1
					n = line[x].n
					line[x].tx = o
				end
			end
		end
		
		for x = 4, 1, -1 do
			if self:moveTile(line[x].fx, line[x].fy, line[x].tx, line[x].ty) then
				changed = true
			end
		end
	end
	
	self:mergeTile()
	return changed
end

function M:moveTop()
	local changed = false
	
	for x = 1, 4 do
		local line = {}
		for y = 1, 4 do
			line[y] = {fx = x, fy = y, tx = x, ty = y, n = self:getNumber(x, y)}
		end

		local n = 0
		local o = 0
		for y = 1, 4 do
			if line[y].n ~= 0 then
				if n == 0 then
					o = o + 1
					n = line[y].n
					line[y].ty = o
				elseif line[y].n == n then
					n = 0
					line[y].ty = o
				else
					o = o + 1
					n = line[y].n
					line[y].ty = o
				end
			end
		end
		
		for y = 1, 4 do
			if self:moveTile(line[y].fx, line[y].fy, line[y].tx, line[y].ty) then
				changed = true
			end
		end
	end
	
	self:mergeTile()
	return changed
end

function M:moveDown()
	local changed = false
	
	for x = 1, 4 do
		local line = {}
		for y = 1, 4 do
			line[y] = {fx = x, fy = y, tx = x, ty = y, n = self:getNumber(x, y)}
		end

		local n = 0
		local o = 5
		for y = 4, 1, -1 do
			if line[y].n ~= 0 then
				if n == 0 then
					o = o - 1
					n = line[y].n
					line[y].ty = o
				elseif line[y].n == n then
					n = 0
					line[y].ty = o
				else
					o = o - 1
					n = line[y].n
					line[y].ty = o
				end
			end
		end
		
		for y = 4, 1, -1 do
			if self:moveTile(line[y].fx, line[y].fy, line[y].tx, line[y].ty) then
				changed = true
			end
		end
	end
	
	self:mergeTile()
	return changed
end

function M:onKeyDown(e)
	local changed = false
	local key = e.key

	if key == 2 then
		changed = self:moveTop()
	elseif key == 3 then
		changed = self:moveDown()
	elseif key == 4 then
		changed = self:moveLeft()
	elseif key == 5 then
		changed = self:moveRight()
	else
		return
	end

	if changed then
		self:randomTile()
		if self:isGameOver() then
			print("game over")
		end
	end
	e.stop = true
end

return M
