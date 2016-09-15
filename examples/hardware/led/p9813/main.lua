local P9813 = require("P9813")

local cled = P9813.new(5, 6, 1)
local h = 0
local c = 200
local l = 100

for i = 1, 100, 1 do
	for h = 0, 768, 25 do
		cled:hclColor(1, h, c, l)
		cled:refresh()
	end
end