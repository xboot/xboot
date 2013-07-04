--if os.getenv('DEBUG_MODE') then require "debugger"() end

local buildin_event = require("org.xboot.buildin.event")
local buildin_timecounter = require("org.xboot.buildin.timecounter")

local class = require("org.xboot.lang.class")
local table = require("org.xboot.lang.table")
local event = require("org.xboot.event.event")
local event_dispatcher = require("org.xboot.event.event_dispatcher")
local display_object = require("org.xboot.display.display_object")
local printr = require("org.xboot.util.printr")

local top  = display_object:new("top")
local obj1 = display_object:new("obj1")
local obj2 = display_object:new("obj2")
local obj3 = display_object:new("obj3")

top:add_child(obj1)
top:add_child(obj2)
top:add_child(obj3)

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

print("=======main test start=================")

local tc = buildin_timecounter.new()
while true do
	print("delta: " .. tc:delta())
	print("uptime: " .. tc:uptime())
end

while true do
	local msg = buildin_event.pump()
	if msg ~= nil then
		local e = event:new(msg.type)
		e.msg = msg
		
		obj1:dispatch_event(e)
	end
end

print("=======main test end=================")
