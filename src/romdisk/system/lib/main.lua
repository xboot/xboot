--if os.getenv('DEBUG_MODE') then require "debugger"() end

local class = require("org.xboot.lang.class")
local table = require("org.xboot.lang.table")
local event = require("org.xboot.event.event")
local event_dispatcher = require("org.xboot.event.event_dispatcher")
local display_object = require("org.xboot.display.display_object")
local _G = _G

local obj1 = display_object:new("obj1")
local obj2 = display_object:new("obj2")
local obj3 = display_object:new("obj3")
local obj4 = display_object:new("obj4")
local obj5 = display_object:new("obj5")
local obj6 = display_object:new("obj6")

obj1:add_child(obj2)
obj2:add_child(obj3)
obj3:add_child(obj4)
obj4:add_child(obj5)
--obj5:add_child(obj6)

if(obj1:contains(nil)) then
	print("ok1")
end

if(obj1:contains(obj3)) then
	print("ok2")
end

if(obj1:contains(obj4)) then
	print("ok3")
end

if(obj1:contains(obj5)) then
	print("ok4")
end

if(obj1:contains(obj6)) then
	print("ok5")
end


local e1 = event("abc")
local e2 = event:new("on123")
local e3 = event:new(event.MOUSE_DOWN)
local e4 = event:new(event.MOUSE_UP)

local ed1 = event_dispatcher:new()

local function onMessage(d, e)
  print("on message ======start")
  print(d)
  print(e)
  print("on message ======end")
  e:stop_propagation()
end

ed1:add_event_listener("111", onMessage, "data111")
ed1:add_event_listener("222", onMessage)
ed1:add_event_listener("333", onMessage, "data333")
ed1:add_event_listener("333", onMessage, "xxxxx")
ed1:add_event_listener("333", onMessage, "ccccccc")
ed1:remove_event_listener("333", onMessage, "data333")

print("=======main test start=================")
print(e1:get_type())
print(e2:get_type())
print(e3:get_type())
print(e4:get_type())
print("=======main test end=================")
