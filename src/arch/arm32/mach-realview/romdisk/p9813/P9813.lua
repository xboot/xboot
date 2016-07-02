local Gpio = require("hardware.gpio")
local M = Class()

function M:init(cin, din)
	self.clk = Gpio.new(cin.name, cin.offset)
	self.dat = Gpio.new(din.name, din.offset)
	self.clk:setDir(Gpio.DIR_OUTPUT)
	self.dat:setDir(Gpio.DIR_OUTPUT)
	self.clk:setValue(1)
	self.dat:setValue(1)
end

function M:sendByte(val)
	for i = 0, 7, 1 do
		if val & 0x80 == 0x80 then
			self.dat:setValue(1)
		else
			self.dat:setValue(0)
		end
		self.clk:setValue(0)
		self.clk:setValue(1)
		val = val << 1
	end
end

function M:setColor(r, g, b)
	local prefix = 0xc0;
	if b & 0x80 == 0 then prefix = prefix | 0x20 end
	if b & 0x40 == 0 then prefix = prefix | 0x10 end
	if g & 0x80 == 0 then prefix = prefix | 0x08 end
	if g & 0x40 == 0 then prefix = prefix | 0x04 end
	if r & 0x80 == 0 then prefix = prefix | 0x02 end
	if r & 0x40 == 0 then prefix = prefix | 0x01 end

	self:sendByte(0)
	self:sendByte(0)
	self:sendByte(0)
	self:sendByte(0)

	self:sendByte(prefix)
	self:sendByte(b)
	self:sendByte(g)
	self:sendByte(r)

	self:sendByte(0)
	self:sendByte(0)
	self:sendByte(0)
	self:sendByte(0)
end

return M