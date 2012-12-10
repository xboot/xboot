local framerate = require "xboot.framerate"
local logger = require "logger"
local dump = require "dump"
local printr = require "printr"
local log = require "lib.log"

function main()
	if xboot.load then xboot.load(arg) end
	local fr = framerate.new()
	while true do
		fr:step()
		if xboot.update then xboot.update(fr:getdelta()) end
		if xboot.draw then xboot.draw() end
		fr:sleep(0.1)
		log.info(fr:getfps())
	end
end

main()

