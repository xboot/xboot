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
	self.stage = DisplayObject.new()
	self.asset = Asset.new()
	self.stopwatch = Stopwatch.new()
	self.timer = Timer.new(1 / 60, 0, function(t, e)
		self.stage:render(self.display, Event.new(Event.ENTER_FRAME))
		self.display:present()
	end)
	self.timer:pause()
	
	application = self
	stage = self.stage
	asset = self.asset
	require("main")
end

function M:quit()
end

function M:loop()
	self.timer:resume()

	while true do
		local info = pump()
		if info ~= nil then
			local e = Event.new(info.type, info)
			self.stage:dispatch(e)
		end

		local elapsed = self.stopwatch:elapsed()
		if elapsed ~= 0 then
			self.stopwatch:reset()
			self.timer:schedule(elapsed)
		end
	end
end

return M
