local M = Class(DisplayObject)

function M:init(fb)
	self.timermanager = TimerManager.new()
	self.display = Display.new(fb)
	self.exiting = false
	if not self.display then
		self.super:init()
	else
		self.super:init(self.display:getSize())
	end
end

function M:getTimerManager()
	return self.timermanager
end

function M:showfps(value)
	self.display:showfps(value)
	return self
end

function M:exit()
	self.exiting = true
	return self
end

function M:loop()
	local Event = Event
	local display = self.display
	local timermanager = self.timermanager
	local stopwatch = Stopwatch.new()

	timermanager:addTimer(Timer.new(1 / 60, 0, function(t, i)
		self:render(display, Event.new(Event.ENTER_FRAME, i))
		display:present()
	end))

	self:addEventListener(Event.KEY_DOWN, function(d, e)
		if e.key == 10 then self:exit() end
	end)

	while not self.exiting do
		local e = Event.pump()
		if e ~= nil then
			self:dispatch(e)
		end

		local elapsed = stopwatch:elapsed()
		if elapsed ~= 0 then
			stopwatch:reset()
			timermanager:schedule(elapsed)
		end
	end
end

return M
