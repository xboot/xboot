local buildin_event = require("org.xboot.buildin.event")
local buildin_timecounter = require("org.xboot.buildin.timecounter")
local buildin_cairo = require("org.xboot.buildin.cairo")

local class = require("org.xboot.lang.class")
local table = require("org.xboot.lang.table")
local event = require("org.xboot.event.event")
local event_dispatcher = require("org.xboot.event.event_dispatcher")
local display_object = require("org.xboot.display.display_object")
local display_image = require("org.xboot.display.display_image")

local timer = require("org.xboot.timer.timer")
local printr = require("org.xboot.util.printr")

print("=======main test start=================")

local runtime = display_object:new("runtime")
local image1 = display_image:new("/romdisk/samples/images/a.png", 0, 0)
local image2 = display_image:new("/romdisk/samples/images/base.png", 100, 100)
local image3 = display_image:new("/romdisk/samples/images/knob.png", 200, 200)

image1:add_event_listener(event.MOUSE_MOVE, function(e, d)
	d:setxy(e.msg.x, e.msg.y)
end, image1)

image1:add_event_listener(event.MOUSE_DOWN, function(e, d)
	d:rotate(15 *  math.pi / 180)
-- d:scale(1.1, 1.1)
--	d.alpha = d.alpha * 0.9
end, image1)

runtime:add_child(image1)
runtime:add_child(image2)
runtime:add_child(image3)

-------------------------------------------------------------------------
local cs = buildin_cairo.xboot_surface_create()
local cr = buildin_cairo.create(cs)
local M_PI = math.pi

local background = buildin_cairo.image_surface_create_from_png("/romdisk/samples/images/background.png");

function draw(d)
	cr:save()
--	cr:set_source_rgb(0.9, 0.9, 0.9)
	cr:set_source_surface(background, 0, 0)
	cr:paint()
	cr:restore()
	
	d:render(cr)
end

------------------------------------------------------------------------------
-- t1
local t1 = timer:new(1 / 60, 0, function(t, e) draw(runtime) end, "t1")

-- main
local tc = buildin_timecounter.new()
while true do
	local msg = buildin_event.pump()
	
	if msg ~= nil then
		local e = event:new(msg.type)
		e.msg = msg
		image1:dispatch_event(e)
	end

	timer:schedule(tc:delta())
end

print("=======main test end=================")
