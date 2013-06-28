local class = require("org.xboot.lang.class")
local event = require("org.xboot.event.event")
local event_dispatcher = require("org.xboot.event.event_dispatcher")

--local _G = _G

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
  --e:stop_propagation()
end

ed1:add_event_listener("111", onMessage, "data111")
ed1:add_event_listener("222", onMessage)
ed1:add_event_listener("333", onMessage, "data333")
ed1:add_event_listener("333", onMessage, "xxxxx")
ed1:add_event_listener("333", onMessage, "ccccccc")

--ed1:remove_event_listener("333", onMessage, "data333")

ed1:dispatch_event(event("333"))

print("=======main test start=================")
print(e1:get_type())
print(e2:get_type())
print(e3:get_type())
print(e4:get_type())
print("=======main test end=================")
