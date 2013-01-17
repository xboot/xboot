local framerate = require "xboot.framerate"
local logger = require "logger"
local dump = require "dump"
local printr = require "printr"
local log = require "lib.log"

local function main()
	if xboot.load then xboot.load(arg) end
	local fr = framerate.new()
	while true do
		fr:step()
		if xboot.update then xboot.update(fr:getdelta()) end
		if xboot.draw then xboot.draw() end
		fr:sleep(0.1)
		log.info(fr:getfps())
		break
	end
end

--[[
local base64 = require "org.xboot.base64"
printr(base64)
a = base64.encode("abc")
b = base64.decode(a)
print(a)
print(b)
]]

--[[
local EVENT = require "org.xboot.event"
local EVENTLISTENER = require "org.xboot.event.listener"
local EVENTDISPATCHER = require "org.xboot.event.dispatcher"

function evcall1(e)
	print("NEW EVENT 1...")
	print(e)
end

function evcall2(e)
	print("NEW EVENT 2...")
	print(e)
end

local el1 = EVENTLISTENER.new(1000, evcall1)
local el2 = EVENTLISTENER.new(1000, evcall2)

EVENTDISPATCHER.add_event_listener(el1);
EVENTDISPATCHER.add_event_listener(el2);

EVENT.new(1000, 10, 20, 3, 4, -1):send()


e1 = EVENT.new(1000, 0, 0, 1, 1, 1)
e2 = EVENT.new(1001, 0, 0, 1, 1, 2)
e3 = EVENT.new(1002, 0, 0, 1, 1, 3)
e4 = EVENT.new(1003, 0, 0, 1, 1, 4)
e5 = EVENT.new(1004, 0, 0, 1, 1, 5)
e = { e1, e2, e3, e4, e5 }

printr(e)

print("------")
print(e1:get("type"))
print(e1:get("timestamp"))
printr(e1:get("event"))
print("------")


printr(EVENT)
print(EVENT.EV_UNKNOWN)
print(EVENT.EV_MOUSE_RAW)
print(e1)

while true do
	EVENTDISPATCHER.run()
end
]]

print("===============")

local EVENT = require "org.xboot.event"
local EVENTLISTENER = require "org.xboot.event.listener"
local EVENTDISPATCHER = require "org.xboot.event.dispatcher"

local function evcallback(e)
	print("evcallback", e)
end

local el = EVENTLISTENER.new(EVENT.EV_MOUSE_RAW, evcallback)

EVENTDISPATCHER.add_event_listener(el);

while true do
	EVENTDISPATCHER.run()
end

print("===============")
