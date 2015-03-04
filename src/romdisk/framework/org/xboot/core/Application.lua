---
-- The 'Application' class contains the execution environment of
-- current application.
--
-- @module Application
local M = Class()

---
-- Creates a new 'Application'.
--
-- @function [parent=#Application] new
-- @return New 'Application' object.
function M:init()
	self.display = Display.new()
	self.stage = DisplayObject.new(self:getScreenSize())
	self.assets = Assets.new()
	self.timermanager = TimerManager.new()
	self.running = false
	self.fpsflag = false
end

function M:getScreenSize()
	local info = self.display:info()
	return info.width, info.height
end

function M:getScreenDensity()
	local info = self.display:info()
	return info.xdpi, info.ydpi
end

function M:getDisplay()
	return self.display
end

function M:getStage()
	return self.stage
end

function M:getAssets()
	return self.assets
end

function M:getTimerManager()
	return self.timermanager
end

function M:fps(enable)
	if enable then
		self.fpsflag = true
	else
		self.fpsflag = false
	end
	return self
end

---
-- Quit application
--
-- @function [parent=#Application] quit
function M:quit()
	self.running = false
	return self
end

---
-- Exec application
--
-- @function [parent=#Application] exec
function M:exec()
	local display = self:getDisplay()
	local stage = self:getStage()
	local timermanager = self:getTimerManager()
	local stopwatch = Stopwatch.new()
	local time = 0
	local frame = 0

	require("main")

	timermanager:addTimer(Timer.new(1 / 60, 0, function(t, i)
		stage:render(display, Event.new(Event.ENTER_FRAME, i))
		display:present()
		if self.fpsflag then
			time = time + i.time
			frame = frame + 1
			if time > 0.5 then
				local fps = math.floor(frame / time + 0.5)
				print(fps)
				time = 0
				frame = 0
			end
		end
	end))

	self.running = true

	while self.running do
		local info = pump()
		if info ~= nil then
			local e = Event.new(info.type, info)
			stage:dispatch(e)
		end

		local elapsed = stopwatch:elapsed()
		if elapsed ~= 0 then
			stopwatch:reset()
			timermanager:schedule(elapsed)
		end
	end
end

return M
