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
Rectangle = require "builtin.rectangle"
Matrix = require "builtin.matrix"
Easing = require "builtin.easing"
Object = require "builtin.object"
Parttern = require "builtin.parttern"
Texture = require "builtin.texture"
Shape = require "builtin.shape"
Font = require "builtin.font"
Display = require "builtin.display"

---
-- Builtin hardware module
--
Hardware = {
	Led = require "builtin.hardware.led",
	Ledtrig = require "builtin.hardware.ledtrig",
}

---
-- External lang module
--
Class = require "org.xboot.lang.Class"

---
-- External core module
--
Timer = require "org.xboot.core.Timer"
Asset = require "org.xboot.core.Asset"
TexturePacker = require "org.xboot.core.TexturePacker"

---
-- External event module
--
Event = require "org.xboot.event.Event"
EventDispatcher = require "org.xboot.event.EventDispatcher"

---
-- External display module
--
DisplayObject = require "org.xboot.display.DisplayObject"
DisplayImage = require "org.xboot.display.DisplayImage"
DisplayShape = require "org.xboot.display.DisplayShape"
DisplayText = require "org.xboot.display.DisplayText"
DisplayBmtext = require "org.xboot.display.DisplayBmtext"

---
-- External widget module
--
Widget = {
	Button = require "org.xboot.widget.Button",
	ImageButton = require "org.xboot.widget.ImageButton",
	Toggle = require "org.xboot.widget.Toggle",
	ImageToogle = require "org.xboot.widget.ImageToggle",
	Led = require "org.xboot.widget.Led",
	ImageLed = require "org.xboot.widget.ImageLed",	
}

---
-- Global runtime
--
runtime = DisplayObject.new()

---
-- Loader function
--
local function loader()
	require("main")

	local pump = require("builtin.event").pump
	local stopwatch = Stopwatch.new()
	local display = Display.new()
	
	Timer.new(1 / 60, 0, function(t, e)
		runtime:render(display, Event.new(Event.ENTER_FRAME))
		display:present()
	end)

	while true do
		local info = pump()	
		if info ~= nil then
			local e = Event.new(info.type, info)
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
