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

local base64 = require "org.xboot.base64"
printr(base64)
a = base64.encode("abc")
b = base64.decode(a)
print(a)
print(b)


local EVENT = require "org.xboot.event"
local EVENTLISTENER = require "org.xboot.eventlistener"
local EVENTDISPATCH = require "org.xboot.eventdispatch"

function evcall(e)
	print("NEW EVENT ...")
	print(e)
end

local el = EVENTLISTENER.new(1000, evcall)
EVENTDISPATCH.addlistener(el);

while true do
	EVENTDISPATCH.run()
end

--printr(xboot)
--printr(arg)
--main()
--assert(false)

