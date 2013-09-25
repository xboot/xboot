---
-- Setting package path
--
package.path = "/romdisk/system/lib/?.lua;/romdisk/system/lib/?/init.lua;./?.lua"
package.cpath = "/romdisk/system/lib/?.so;/romdisk/system/lib/loadall.so;./?.so"

---
-- Builtin module
--
print = require("builtin.logger").print
Stopwatch = require "builtin.stopwatch"
Base64 = require "builtin.base64"
Display = require "builtin.display"
Cairo = require "builtin.cairo"
Matrix = require "builtin.matrix"
Font = require "builtin.font"
Parttern = require "builtin.parttern"
Texture = require "builtin.texture"
Shape = require "builtin.shape"
Hardware = {
	led = require "builtin.hardware.led",
	ledtrig = require "builtin.hardware.ledtrig",
}

---
-- External base module
--
Class = require "base.Class"
Timer = require "base.Timer"
Event = require "base.Event"
EventDispatcher = require "base.EventDispatcher"
TexturePacker = require "base.TexturePacker"
DisplayObject = require "base.DisplayObject"
DisplayImage = require "base.DisplayImage"
DisplayShape = require "base.DisplayShape"
DisplayText = require "base.DisplayText"
DisplayBmtext = require "base.DisplayBmtext"

---
-- Global runtime
--
runtime = DisplayObject:new()

---
-- Loader function
--
local function loader()
	require("main")

	local pump = require("builtin.event").pump
	local stopwatch = Stopwatch.new()
	local display = Display.new()
	
	Timer:new(1 / 60, 0, function(t, e)
		runtime:render(display, Event:new(Event.ENTER_FRAME))
		display:present()
	end)

	while true do
		local info = pump()	
		if info ~= nil then
			local e = Event:new(info.type, info)
			runtime:dispatch(e)
		end
	
		local elapsed = stopwatch:elapsed()
		if elapsed ~= 0 then
			stopwatch:reset()
			Timer:schedule(elapsed)
		end
	end
end

---
-- Message handler
--
local function handler(msg, layer)
	print((debug.traceback("ERROR: " .. tostring(msg), 1 + (layer or 1)):gsub("\n[^\n]+$", "")))
end

return function()
	local res, ret = xpcall(loader, handler)
	if not res then return -1 end
	return tonumber(ret) or -1
end
