---
-- Setting package path
--
package.path = "./?/init.lua;./?.lua"
package.cpath = "./?.so"

---
-- Builtin module
--
Json = require "builtin.json"

print = require("builtin.logger").print
pump = require("builtin.event").pump

Stopwatch = require "builtin.stopwatch"
Base64 = require "builtin.base64"
Matrix = require "builtin.matrix"
Easing = require "builtin.easing"
Object = require "builtin.object"
Pattern = require "builtin.pattern"
Texture = require "builtin.texture"
Ninepatch = require "builtin.ninepatch"
Shape = require "builtin.shape"
Font = require "builtin.font"
Display = require "builtin.display"

---
-- Builtin hardware module
--
Hardware = {
	uart = require "builtin.hardware.uart",
	i2c = require "builtin.hardware.i2c",
	gpio = require "builtin.hardware.gpio",
	pwm = require "builtin.hardware.pwm",
	led = require "builtin.hardware.led",
	ledtrig = require "builtin.hardware.ledtrig",
	buzzer = require "builtin.hardware.buzzer",
	watchdog = require "builtin.hardware.watchdog",
}

---
-- External lang module
--
Class = require "org.xboot.lang.Class"

---
-- External core module
--
Assets = require "org.xboot.core.Assets"
TexturePacker = require "org.xboot.core.TexturePacker"
Application = require "org.xboot.core.Application"

---
-- External timer module
--
Timer = require "org.xboot.timer.Timer"
TimerManager = require "org.xboot.timer.TimerManager"

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
DisplayImageMask = require "org.xboot.display.DisplayImageMask"
DisplayNinepatch = require "org.xboot.display.DisplayNinepatch"
DisplayShape = require "org.xboot.display.DisplayShape"
DisplayText = require "org.xboot.display.DisplayText"

---
-- External widget module
--
Widget = {
	Button = require "org.xboot.widget.Button",
	CheckBox = require "org.xboot.widget.CheckBox",
	RadioButton = require "org.xboot.widget.RadioButton",
	Stepper = require "org.xboot.widget.Stepper",
	Slider = require "org.xboot.widget.Slider",
}

---
-- Global variable
--
application = nil

---
-- Loader function
--
local function loader()
	application = Application.new()
	application:exec()
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
