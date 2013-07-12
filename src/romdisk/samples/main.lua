local buildin_event = require("org.xboot.buildin.event")
local buildin_timecounter = require("org.xboot.buildin.timecounter")
local cairo = require("org.xboot.buildin.cairo")

local class = require("org.xboot.lang.class")
local table = require("org.xboot.lang.table")
local event = require("org.xboot.event.event")
local event_dispatcher = require("org.xboot.event.event_dispatcher")
local display_object = require("org.xboot.display.display_object")
local timer = require("org.xboot.timer.timer")
local printr = require("org.xboot.util.printr")

print("=======main test start=================")
local runtime = display_object:new("runtime")

local d1 = display_object:new("d1")
d1:add_event_listener(event.MOUSE_MOVE, function(e, d) d:setxy(e.msg.x, e.msg.y) end, d1)

-------------------------------------------------------------------------
--local cs = cairo.image_surface_create(cairo.FORMAT_ARGB32, 400, 400)
local cs = cairo.xboot_surface_create()
local cr = cairo.create(cs)
	
function draw(d)
	cr:save()
	cr:set_source_rgb(0.9, 0.9, 0.9)
	cr:paint()
	cr:restore()
	
	cr:set_line_width(6)
	cr:rectangle(d:getx(), d:gety(), d:getw(), d:geth())
	cr:set_source_rgb(0, 0.7, 0)
	cr:fill_preserve()
	cr:set_source_rgb(0, 0, 0)
	cr:stroke()
	
--	cs:show()
--	collectgarbage("collect")
--	collectgarbage("step")
end

------------------------------------------------------------------------------
-- t1
local t1 = timer:new(1 / 60, 0, function(t, e)
	draw(d1)
end, "t1")

-- main
local tc = buildin_timecounter.new()
while true do
	local msg = buildin_event.pump()
	
	if msg ~= nil then
		local e = event:new(msg.type)
		e.msg = msg
		d1:dispatch_event(e)
	end

	timer:schedule(tc:delta())
end

print("=======main test end=================")
