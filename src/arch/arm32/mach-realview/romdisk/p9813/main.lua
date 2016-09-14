local P9813 = require("P9813")

local cled = P9813.new(5, 6, 1)

for r = 0, 255, 8 do
	for g = 0, 255, 8 do
		for b = 0, 255, 8 do
			cled:setColor(1, r, g, b)
			cled:refresh()
		end
	end
end
