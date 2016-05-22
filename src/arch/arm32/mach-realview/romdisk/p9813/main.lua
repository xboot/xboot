local P9813 = require("P9813")

local cled = P9813.new({name = "GPIO", offset = 20}, {name = "GPIO", offset = 21})

function delay(time)
	local stopwatch = Stopwatch.new()
	local elapsed = 0
	stopwatch:reset()
	while elapsed < time do
		elapsed = stopwatch:elapsed()
	end
end

for r = 0, 255, 8 do
	for g = 0, 255, 8 do
		for b = 0, 255, 8 do
			cled:setColor(r, g, b)
			delay(0.01)
		end
	end
end
