--if os.getenv('DEBUG_MODE') then require "debugger"() end

local class = require("org.xboot.lang.class")
local table = require("org.xboot.lang.table")
local event = require("org.xboot.event.event")
local event_dispatcher = require("org.xboot.event.event_dispatcher")
local display_object = require("org.xboot.display.display_object")

local top  = display_object:new("top")
local obj1 = display_object:new("obj1")
local obj2 = display_object:new("obj2")
local obj3 = display_object:new("obj3")
local obj4 = display_object:new("obj4")
local obj5 = display_object:new("obj5")
local obj6 = display_object:new("obj6")

top:add_child(obj1)
top:add_child(obj2)
top:add_child(obj3)
top:add_child(obj4)
top:add_child(obj5)
top:add_child(obj6)

obj1:add_event_listener("et-obj1", 
  function(e)
    print(e:get_type() .. ' [OBJ1]')
    obj2:dispatch_event(event:new("et-obj2"))
    obj3:dispatch_event(event:new("et-obj3"))
  end)

obj2:add_event_listener("et-obj2", function(e) print(e:get_type() .. ' [OBJ2]') end)
obj3:add_event_listener("et-obj3", function(e) print(e:get_type() .. ' [OBJ3]') end)  
obj4:add_event_listener("et-obj4", function(e) print(e:get_type() .. ' [OBJ4]') end)
obj5:add_event_listener("et-obj5", function(e) print(e:get_type() .. ' [OBJ5]') end)
obj6:add_event_listener("et-obj6", function(e) print(e:get_type() .. ' [OBJ6]') end)  

print("=======main test start=================")
obj1:dispatch_event(event:new("et-obj1"))
print("=======main test end=================")
