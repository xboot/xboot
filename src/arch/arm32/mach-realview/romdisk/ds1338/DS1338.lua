local I2c = require("xboot.hardware.i2c")
local M = Class()

function M:init(bus)
	self.i2c = I2c.new(bus, 0x68)
end

function M:bcd2hex(bcd)
	return (bcd // 16 * 10 + bcd % 16)
end

function M:hex2bcd(hex)
	return (hex // 10 * 16 + hex % 10)
end

function M:write(reg, val)
	self.i2c:write(string.char(reg, val))
	return self
end

function M:read(reg)
	self.i2c:write(string.char(reg))
	return string.byte(self.i2c:read(1) or "0", 1, 1)
end

function M:getTime()
	local second = self:bcd2hex(self:read(0))
	local minute = self:bcd2hex(self:read(1))
	local hour = self:bcd2hex(self:read(2))
	local day = self:bcd2hex(self:read(3))
	local date = self:bcd2hex(self:read(4))
	local month = self:bcd2hex(self:read(5))
	local year = self:bcd2hex(self:read(6))

	return {
		second = second,
		minute = minute,
		hour = hour,
		day = day,
		date = date,
		month = month,
		year = year
	}
end

return M