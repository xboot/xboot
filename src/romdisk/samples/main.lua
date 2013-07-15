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

local function on_mouse_down(e, d)
	if d:hit_test_point(e.msg.x, e.msg.y) then
		d.isfocus = true
		--d:tofront()
		
		d.x0 = e.msg.x
		d.y0 = e.msg.y
		e:stop_propagation()
	end
end

local function on_mouse_move(e, d)
	if d.isfocus then
		local dx = e.msg.x - d.x0
		local dy = e.msg.y - d.y0
		
		d:translate(dx, dy)
		d.x0 = e.msg.x
		d.y0 = e.msg.y
		e:stop_propagation()
	end
end

local function on_mouse_up(e, d)
	if d.isfocus then
		d.isfocus = false
		e:stop_propagation()
	end
end

for i=1,5 do
	local img = display_image:new("/romdisk/samples/images/" .. i ..".png", math.random(0, 800 - 100), math.random(0, 480 - 100))
	img.alpha = 1
	
	img:add_event_listener(event.MOUSE_DOWN, on_mouse_down, img)
	img:add_event_listener(event.MOUSE_MOVE, on_mouse_move, img)
	img:add_event_listener(event.MOUSE_UP, on_mouse_up, img)
	
	runtime:add_child(img)
end

local cursor = display_image:new("/romdisk/samples/images/cursor.png", 0, 0)
cursor:add_event_listener(event.MOUSE_DOWN, function(e, d) d:setxy(e.msg.x, e.msg.y) end, cursor)
cursor:add_event_listener(event.MOUSE_MOVE, function(e, d) d:setxy(e.msg.x, e.msg.y) end, cursor)
cursor:add_event_listener(event.MOUSE_UP, function(e, d) d:setxy(e.msg.x, e.msg.y) end, cursor)
runtime:add_child(cursor)

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
