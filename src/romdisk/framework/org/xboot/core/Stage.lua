local M = class(DisplayObject)

function M:init(fb)
	self.super:init()
	self.display = Display.new(fb)
	self.timermanager = TimerManager.new()
	self.looping = false
	self.super:init(self:getScreenSize())
end

function M:getScreenSize()
	local info = self.display:info()
	return info.width, info.height
end

function M:getScreenDensity()
	local info = self.display:info()
	return info.xdpi, info.ydpi
end

function M:getTimerManager()
	return self.timermanager
end

function M:showfps(value)
	self.display:showfps(value)
	return self
end

function M:quit()
	self.looping = false
	return self
end

function M:loop()
	local display = self.display
	local timermanager = self.timermanager
	local stopwatch = Stopwatch.new()

	timermanager:addTimer(Timer.new(1 / 60, 0, function(t, i)
		self:render(display, Event.new(Event.ENTER_FRAME, i))
		display:present()
	end))

	self.looping = true
	while self.looping do
		local info = pump()
		if info ~= nil then
			local e = Event.new(info.type, info)
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
