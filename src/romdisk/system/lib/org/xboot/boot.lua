package.path = "/romdisk/system/lib/?.lua;/romdisk/system/lib/?/init.lua;./?.lua"
package.cpath = "/romdisk/system/lib/?.so;/romdisk/system/lib/loadall.so;./?.so"

print = require("org.xboot.buildin.logger").print

buildin_event = require "org.xboot.buildin.event"
buildin_stopwatch = require "org.xboot.buildin.stopwatch"
buildin_base64 = require "org.xboot.buildin.base64"
buildin_cairo = require "org.xboot.buildin.cairo"

buildin_hardware = {
	led = require "org.xboot.buildin.hardware.led",
	ledtrig = require "org.xboot.buildin.hardware.ledtrig",
}

Class = require "org.xboot.lang.Class"
Timer = require "org.xboot.timer.Timer"
Event = require "org.xboot.event.Event"
EventDispatcher = require "org.xboot.event.EventDispatcher"
Texture = require "org.xboot.display.Texture"
DisplayObject = require "org.xboot.display.DisplayObject"
DisplayImage = require "org.xboot.display.DisplayImage"

runtime = DisplayObject:new()

local function loader()
	require("main")

	local stopwatch = buildin_stopwatch.new()
	local cs = {
		buildin_cairo.xboot_surface_create(),
		buildin_cairo.xboot_surface_create(),
	}
	local cr = {
		buildin_cairo.create(cs[1]),
		buildin_cairo.create(cs[2]),
	}
	local cidx = 1;
	
	Timer:new(1 / 60, 0, function(t, e)
		cidx = cidx + 1
		if cidx > #cs then
			cidx = 1
		end
	
		runtime:render(cr[cidx], Event:new(Event.ENTER_FRAME))
		cs[cidx]:present()
	end)

	while true do
		local info = buildin_event.pump()	
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

local function handler(msg, layer)
	print((debug.traceback("ERROR: " .. tostring(msg), 1 + (layer or 1)):gsub("\n[^\n]+$", "")))
end

return function()
	local res, ret = xpcall(loader, handler)
	if not res then return -1 end
	return tonumber(ret) or -1
end
