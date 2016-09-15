local DS1338 = require("DS1338")

local ds1338 = DS1338.new("versatile-i2c.0")
local t = ds1338:getTime()
print(t)

ds1338:writeRam(0, 0x55)
print(ds1338:readRam(0))
