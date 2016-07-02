local I2C = require("hardware.i2c")
local M = Class()

local REG_SECOND = 0x0
local REG_MINUTE = 0x1
local REG_HOUR   = 0x2
local REG_DAY    = 0x3
local REG_DATE   = 0x4
local REG_MONTH  = 0x5
local REG_YEAR   = 0x6
local REG_CTRL   = 0x7
local REG_RAM    = 0x8

function M:init(bus)
	self.i2c = I2C.new(bus, 0x68)
	if self:readReg(REG_CTRL) & 0x20 ~= 0 then
		self:setTime({ second = 0, minute = 0, hour = 0, day = 5, date =  1, month = 1, year = 2016 })
		self:writeReg(REG_CTRL, 0x0)
	end
end

function M:bcd2bin(bcd)
	return (bcd // 16 * 10 + bcd % 16)
end

function M:bin2bcd(bin)
	return (bin // 10 * 16 + bin % 10)
end

function M:writeReg(reg, val)
	self.i2c:write(string.char(reg, val))
	return self
end

function M:readReg(reg)
	self.i2c:write(string.char(reg))
	return string.byte(self.i2c:read(1) or "0", 1, 1)
end

function M:writeRam(offset, val)
	if type(offset) == "number" and offset >= 0 and offset <= 55 then
		self:writeReg(REG_RAM + offset, val)
	end
	return self
end

function M:readRam(offset)
	if type(offset) == "number" and offset >= 0 and offset <= 55 then
		return self:readReg(REG_RAM + offset)
	else
		return nil
	end
end

--[[
 -------------------------------------------------------------
| OUT |     | OSF | SQWE |     |     | RS1 | RS0 | SQW Output |
 -------------------------------------------------------------
|  X  |     |  0  |   1  |     |     |  0  |  0  |        1Hz |
|  X  |     |  0  |   1  |     |     |  0  |  1  |   4.096kHz |
|  X  |     |  0  |   1  |     |     |  1  |  0  |   8.192kHz |
|  X  |     |  0  |   1  |     |     |  1  |  1  |  32.768kHz |
|  0  |     |  0  |   0  |     |     |  X  |  X  |          0 |
|  1  |     |  0  |   0  |     |     |  X  |  X  |          1 |
 -------------------------------------------------------------
--]]
function M:squareWave(hz)
	local ctrl = 0x0
	if hz == 1 then
		ctrl = 0x10
	elseif hz == 4096 then
		ctrl = 0x11
	elseif hz == 8192 then
		ctrl = 0x12
	elseif hz == 32768 then
		ctrl = 0x13
	end
	self:writeReg(REG_CTRL, ctrl)
	return self
end

function M:outputLevel(level)
	local ctrl = 0x0
	if type(level) == "number" and level ~= 0 then
		ctrl = 0x80
	end
	self:writeReg(REG_CTRL, ctrl)
	return self
end

function M:setTime(t)
	local t = t or {}
	if t.second ~= nil and t.second >= 0 and t.second <= 59 then
		self:writeReg(REG_SECOND, self:bin2bcd(t.second) & 0x7f)
	end
	if t.minute ~= nil and t.minute >= 0 and t.minute <= 59 then
		self:writeReg(REG_MINUTE, self:bin2bcd(t.minute) & 0x7f)
	end
	if t.hour ~= nil and t.hour >= 0 and t.hour <= 23 then
		self:writeReg(REG_HOUR, self:bin2bcd(t.hour) & 0x3f)
	end
	if t.day ~= nil and t.day >= 0 and t.day <= 6 then
		self:writeReg(REG_DAY, self:bin2bcd(t.day + 1) & 0x7)
	end
	if t.date ~= nil and t.date >= 0 and t.date <= 31 then
		self:writeReg(REG_DATE, self:bin2bcd(t.date) & 0x3f)
	end
	if t.month ~= nil and t.month >= 0 and t.month <= 12 then
		self:writeReg(REG_MONTH, self:bin2bcd(t.month) & 0x1f)
	end
	if t.year ~= nil and t.year >= 2000 and t.year <= 2000 + 99 then
		self:writeReg(REG_YEAR, self:bin2bcd(t.year - 2000) & 0xff)
	end
	return self
end

function M:getTime()
	return {
		second = self:bcd2bin(self:readReg(REG_SECOND) & 0x7f),
		minute = self:bcd2bin(self:readReg(REG_MINUTE) & 0x7f),
		hour = self:bcd2bin(self:readReg(REG_HOUR) & 0x3f),
		day = self:bcd2bin(self:readReg(REG_DAY) & 0x7) - 1,
		date = self:bcd2bin(self:readReg(REG_DATE) & 0x3f),
		month = self:bcd2bin(self:readReg(REG_MONTH) & 0x1f),
		year = self:bcd2bin(self:readReg(REG_YEAR) & 0xff) + 2000,
	}
end

return M