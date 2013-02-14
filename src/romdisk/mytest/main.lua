local framerate = require "xboot.framerate"
local logger = require "logger"
local dump = require "dump"
local printr = require "printr"
local log = require "lib.log"
--local cairo = require "org.xboot.cairo"
--local CAIRO = cairo

local function main()
	local w = 320
	local h = 240

	local cs = cairo.image_surface_create(CAIRO.FORMAT_ARGB32, w, h)
	local cr = cairo.create (cs)

	cairo.save(cr)
	cairo.set_source_rgb(cr, 1, 0, 1)
	cairo.paint(cr)
	cairo.restore(cr)

	cairo.lcd(cs)
	while true do end
end

--main()
