local stage = Stage.new()

local c = 1
timermanager:addTimer(Timer.new(0.1, 0, function(t, i)
  c = c + 1
  print("timer: " .. c)
  end))

stage:showfps(true)
stage:loop()
