local buildin_event = require("org.xboot.buildin.event")
local buildin_timecounter = require("org.xboot.buildin.timecounter")

local class = require("org.xboot.lang.class")
local table = require("org.xboot.lang.table")
local event = require("org.xboot.event.event")
local event_dispatcher = require("org.xboot.event.event_dispatcher")
local display_object = require("org.xboot.display.display_object")
local timer = require("org.xboot.timer.timer")
local printr = require("org.xboot.util.printr")


print("=======main test start=================")

-- obj1
local obj1 = display_object:new("obj1")

local function obj1_on_mouse_down(e)
    print("DOWN " .. " [" .. e.msg.x .. "," .. e.msg.y .. "]")
end

local function obj1_on_mouse_up(e)
	print("UP   " .. " [" .. e.msg.x .. "," .. e.msg.y .. "]")
end

local function obj1_on_mouse_move(e)
	print("MOVE " .. " [" .. e.msg.x .. "," .. e.msg.y .. "]")
end

obj1:add_event_listener(event.MOUSE_DOWN, obj1_on_mouse_down)
obj1:add_event_listener(event.MOUSE_UP, obj1_on_mouse_up)
obj1:add_event_listener(event.MOUSE_MOVE, obj1_on_mouse_move)

-- t1
local function t1_on_timer(t, e)
    print("t1, " .. e.count)
end
local t1 = timer:new(1, 0, t1_on_timer)

-- t2
local function t2_on_timer(t, e)
    print("t2, " .. e.count)
end
local t2 = timer:new(1, 3, t2_on_timer)

-- main
local tc = buildin_timecounter.new()
while true do
	local msg = buildin_event.pump()
	
	if msg ~= nil then
		local e = event:new(msg.type)
		e.msg = msg
		obj1:dispatch_event(e)
	end

	timer:schedule(tc:delta())
end

print("=======main test end=================")
