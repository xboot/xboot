package.path = "/romdisk/system/lib/?.lua;/romdisk/system/lib/?/init.lua;./?.lua"
package.cpath = "/romdisk/system/lib/?.so;/romdisk/system/lib/loadall.so;./?.so"

buildin_event = require("org.xboot.buildin.event")
buildin_logger = require("org.xboot.buildin.logger")
buildin_stopwatch = require("org.xboot.buildin.stopwatch")
buildin_base64 = require("org.xboot.buildin.base64")
buildin_cairo = require("org.xboot.buildin.cairo")
print = buildin_logger.print

class = require("org.xboot.lang.class")
timer = require("org.xboot.timer.timer")
event = require("org.xboot.event.event")
event_dispatcher = require("org.xboot.event.event_dispatcher")
display_object = require("org.xboot.display.display_object")
display_image = require("org.xboot.display.display_image")

runtime = display_object:new()

local function loader()
	require("main")

	local sw = buildin_stopwatch.new()
	local cs = {
		buildin_cairo.xboot_surface_create(),
		buildin_cairo.xboot_surface_create(),
	}
	local cr = {
		buildin_cairo.create(cs[1]),
		buildin_cairo.create(cs[2]),
	}
	local cidx = 1;
	
	timer:new(1 / 60, 0, function(t, e)
		cidx = cidx + 1
		if cidx > #cs then
			cidx = 1
		end
	
		runtime:render(cr[cidx], event:new(event.ENTER_FRAME))
		cs[cidx]:present()
	end)

	while true do
		local info = buildin_event.pump()	
		if info ~= nil then
			local e = event:new(info.type, info)
			runtime:dispatch(e)
		end
	
		local elapsed = sw:elapsed()
		if elapsed ~= 0 then
			sw:reset()
			timer:schedule(elapsed)
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
