local c = 1

stage:addTimer(Timer.new(0.1, 0, function(t)
	c = c + 1
	print("timer: " .. c)
end))

