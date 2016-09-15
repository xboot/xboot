local MAX7219 = require("MAX7219")

local max7219 = MAX7219.new(5, 6, 26)

for i = 1, 10000, 1 do
	max7219:display(string.format("%8d", i))
end