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

local M_PI = math.pi
local sin = math.sin
local cos = math.cos

local count = 0;

function draw(e)
    count = count + 1
    if(count >= 360) then
    	count = 0
    end
    
	cr:save()
	cr:set_source_rgb(0.9, 0.9, 0.9)
	cr:paint()
	cr:restore()
	
    local xc = 128.0;
    local yc = 128.0;
    local radius = 100.0;
    local angle1 = count  * (M_PI / 180.0);
    local angle2 = 180.0 * (M_PI / 180.0);

    cr:set_line_width(10.0)
    cr:arc(xc, yc, radius, angle1, angle2);
    cr:stroke();

    cr:set_source_rgba(1, 0.2, 0.2, 0.6);
    cr:set_line_width(6.0);

    cr:arc(xc, yc, 10.0, 0, 2*M_PI);
    cr:fill();

    cr:arc(xc, yc, radius, angle1, angle1);
    cr:line_to(xc, yc);
    cr:arc(xc, yc, radius, angle2, angle2);
    cr:line_to(xc, yc);
    cr:stroke();
    if(e.count % 60 == 0) then
    	print(1/e.time)
    end
--	collectgarbage("collect")
--	collectgarbage("step")	
end

------------------------------------------------------------------------------
-- t1
local t1 = timer:new(1 / 10, 0, function(t, e)
	draw(e)
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
