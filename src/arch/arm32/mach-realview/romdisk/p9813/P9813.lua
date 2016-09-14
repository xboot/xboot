local GPIO = require("hardware.gpio")
local M = Class()

function M:init(cpin, dpin, n)
	self.clk = GPIO.new(cpin)
	self.dat = GPIO.new(dpin)
	self.num = n or 1

	self.buf = {}
	for i = 1, self.num, 1 do
		self.buf[i] = {r = 0, g = 0, b = 0}
	end

	self.clk:setDir(GPIO.DIR_OUTPUT)
	self.dat:setDir(GPIO.DIR_OUTPUT)
	self.clk:setValue(1)
	self.dat:setValue(1)
end

function M:sendByte(byte)
	for i = 1, 8, 1 do
		if byte & 0x80 == 0x80 then
			self.dat:setValue(1)
		else
			self.dat:setValue(0)
		end
		self.clk:setValue(0)
		self.clk:setValue(1)
		byte = byte << 1
	end
end

function M:setColor(i, r, g, b)
	self.buf[i].r = r or self.buf[i].r
	self.buf[i].g = g or self.buf[i].g
	self.buf[i].b = b or self.buf[i].b
end

function M:refresh()
	self:sendByte(0)
	self:sendByte(0)
	self:sendByte(0)
	self:sendByte(0)

	for i = 1, self.num, 1 do
		local r = self.buf[i].r
		local g = self.buf[i].g
		local b = self.buf[i].b
		local prefix = 0xc0;
		if b & 0x80 == 0 then prefix = prefix | 0x20 end
		if b & 0x40 == 0 then prefix = prefix | 0x10 end
		if g & 0x80 == 0 then prefix = prefix | 0x08 end
		if g & 0x40 == 0 then prefix = prefix | 0x04 end
		if r & 0x80 == 0 then prefix = prefix | 0x02 end
		if r & 0x40 == 0 then prefix = prefix | 0x01 end

		self:sendByte(prefix)
		self:sendByte(b)
		self:sendByte(g)
		self:sendByte(r)
	end

	self:sendByte(0)
	self:sendByte(0)
	self:sendByte(0)
	self:sendByte(0)
end

return M