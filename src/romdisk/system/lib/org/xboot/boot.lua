package.path = "/romdisk/system/lib/?.lua;/romdisk/system/lib/?/init.lua;./?.lua"
package.cpath = "/romdisk/system/lib/?.so;/romdisk/system/lib/loadall.so;./?.so"

buildin_event = require("org.xboot.buildin.event")
buildin_logger = require("org.xboot.buildin.logger")
buildin_timecounter = require("org.xboot.buildin.timecounter")
buildin_base64 = require("org.xboot.buildin.base64")
buildin_cairo = require("org.xboot.buildin.cairo")
print = buildin_logger.print

class = require("org.xboot.lang.class")
timer = require("org.xboot.timer.timer")
event = require("org.xboot.event.event")
event_dispatcher = require("org.xboot.event.event_dispatcher")
display_object = require("org.xboot.display.display_object")
display_image = require("org.xboot.display.display_image")

local function loader()
	require("main")
end

local function handler(msg, layer)
	print((debug.traceback("ERROR: " .. tostring(msg), 1 + (layer or 1)):gsub("\n[^\n]+$", "")))
end

return function()
	local res, ret = xpcall(loader, handler)
	if not res then return -1 end
	return tonumber(ret) or -1
end
