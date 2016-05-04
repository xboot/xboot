local DS1338 = require("DS1338")

Debugger()

local ds1338 = DS1338.new("versatile-i2c.0")
local t = ds1338:getTime()
print(t)


--[[
local i2c = require("xboot.hardware.i2c")

local ds1338 = i2c.new("versatile-i2c.0", 0x68)

ds1338:write(string.char(0x00))
local a = ds1338:read(4)

for i = 1, string.len(a) do
  print(string.format("0x%02x", string.byte(a, i, i)))
end
--]]