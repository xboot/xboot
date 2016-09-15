local GPIO = require("hardware.gpio")
local M = Class()

function M:init(dat, clk, cs)
	self.dat = GPIO.new(dat)
	self.clk = GPIO.new(clk)
	self.cs = GPIO.new(cs)
	self.dat:setDir(GPIO.DIR_OUTPUT)
	self.clk:setDir(GPIO.DIR_OUTPUT)
	self.cs:setDir(GPIO.DIR_OUTPUT)
	self.dat:setValue(1)
	self.clk:setValue(1)
	self.cs:setValue(1)

	self:send(0x0f, 0x00)
	self:send(0x09, 0xff)
	self:send(0x0a, 0x0f)
	self:send(0x0b, 0x07)
	self:send(0x0c, 0x01)
end

function M:send(addr, val)
	self.cs:setValue(0)
	for i = 1, 8, 1 do
		if addr & 0x80 == 0x80 then
			self.dat:setValue(1)
		else
			self.dat:setValue(0)
		end
		self.clk:setValue(0)
		self.clk:setValue(1)
		addr = addr << 1
	end
	for i = 1, 8, 1 do
		if val & 0x80 == 0x80 then
			self.dat:setValue(1)
		else
			self.dat:setValue(0)
		end
		self.clk:setValue(0)
		self.clk:setValue(1)
		val = val << 1
	end
	self.cs:setValue(1)
end

function M:setIntensity(intensity)
	self:send(0x0a, intensity)
end

function M:setScanLimit(limit)
	if limit < 1 then
		limit = 1
	elseif limit > 8 then
		limit = 8
	end
	self:send(0x0b, limit - 1)
end

function M:shutdown(mode)
	self:send(0x0c, mode and 1 or 0)
end

function M:display(s)
	local l = #s
	for i = 0, l - 1, 1 do
		self:send(i + 1, string.byte(s, l - i, l - i))
	end
end

return M