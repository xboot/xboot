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


----------------------------------------------------------------------------------

local runtime = display_object:new("runtime")
local image1 = display_image:new("/romdisk/samples/images/a.png", 0, 0)
local image2 = display_image:new("/romdisk/samples/images/base.png", 100, 100)
local image3 = display_image:new("/romdisk/samples/images/knob.png", 200, 200)

local image4 = display_image:new("/romdisk/samples/images/knob.png", 300, 200)
local image5 = display_image:new("/romdisk/samples/images/knob.png", 400, 200)
local image6 = display_image:new("/romdisk/samples/images/knob.png", 500, 200)

image1:add_event_listener(event.MOUSE_DOWN, function(e, d) print("image1 " .. e.msg.x .. "," .. e.msg.y) end)
image2:add_event_listener(event.MOUSE_DOWN, function(e, d) print("image2 " .. e.msg.x .. "," .. e.msg.y) end)
image3:add_event_listener(event.MOUSE_DOWN, function(e, d) print("image3 " .. e.msg.x .. "," .. e.msg.y) end)

image4:add_event_listener(event.MOUSE_DOWN, function(e, d) print("image4 " .. e.msg.x .. "," .. e.msg.y) end)
image5:add_event_listener(event.MOUSE_DOWN, function(e, d) print("image5 " .. e.msg.x .. "," .. e.msg.y) end)
image6:add_event_listener(event.MOUSE_DOWN, function(e, d) print("image6 " .. e.msg.x .. "," .. e.msg.y) end)

image1:add_event_listener(event.MOUSE_MOVE, function(e, d)
	d:setxy(e.msg.x, e.msg.y)
end, image1)

image1:add_event_listener(event.MOUSE_DOWN, function(e, d)
	d:rotate(10 * math.pi / 180)
end, image1)

image1:add_event_listener(event.MOUSE_UP, function(e, d)
	d:scale(1.1, 1.1)
end, image1)

local t = 0.1
image2:add_event_listener(event.ENTER_FRAME, function(e, d)

	d.alpha = d.alpha + t
	if(d.alpha >=0.9 ) then
		t = -0.1
	elseif(d.alpha <=0.1 ) then
		t = 0.1
	end
end, image2)

runtime:add_child(image1)
runtime:add_child(image2)
runtime:add_child(image3)

image3:add_child(image4)
image3:add_child(image5)
image5:add_child(image6)

------------------- main --------------------------------
local tc = buildin_timecounter.new()
local cs = buildin_cairo.xboot_surface_create()
local cr = buildin_cairo.create(cs)
local background = buildin_cairo.image_surface_create_from_png("/romdisk/samples/images/background.png");

timer:new(1 / 60, 0, function(t, e)
	cr:save()
--	cr:set_source_rgb(0.9, 0.9, 0.9)
	cr:set_source_surface(background, 0, 0)
	cr:paint()
	cr:restore()

	runtime:render(cr, event:new(event.ENTER_FRAME))
end)

while true do
	local msg = buildin_event.pump()	
	if msg ~= nil then
		local e = event:new(msg.type)
		e.msg = msg
		runtime:dispatch(e)
	end

	timer:schedule(tc:delta())
end
