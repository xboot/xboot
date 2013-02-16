local M = {}

function M.test()
	local cairo = require "org.xboot.cairo"
	local M_PI = math.pi
	
	local cs = cairo.image_surface_create(cairo.FORMAT_ARGB32, 400, 400)
	local cr = cairo.create(cs)
	
	cr:save()
	cr:set_source_rgb(0.9, 0.9, 0.9)
	cr:paint()
	cr:restore()
	
    cr:arc(128.0, 128.0, 76.8, 0, 2 * M_PI)
    cr:clip()

    cr:new_path()
    cr:rectangle(0, 0, 256, 256)
    cr:fill()
    cr:set_source_rgb(0, 1, 0)
    cr:move_to(0, 0)
    cr:line_to(256, 256)
    cr:move_to(256, 0)
    cr:line_to(0, 256)
    cr:set_line_width(10.0)
    cr:stroke()
	
	cs:show()
	collectgarbage("collect")
	collectgarbage("step")
end

return M
