local M = {}

function M.test()
	local cairo = require "org.xboot.cairo"
	local M_PI = math.pi
	local sqrt = math.sqrt
	
	local cs = cairo.image_surface_create(cairo.FORMAT_ARGB32, 400, 400)
	local cr = cairo.create(cs)
	
	cr:save()
	cr:set_source_rgb(0.9, 0.9, 0.9)
	cr:paint()
	cr:restore()
	
	cr:move_to(50.0, 75.0);
	cr:line_to(200.0, 75.0);

	cr:move_to(50.0, 125.0);
	cr:line_to(200.0, 125.0);

	cr:move_to(50.0, 175.0);
	cr:line_to(200.0, 175.0);

	cr:set_line_width(30.0);
	cr:set_line_cap(cairo.LINE_CAP_ROUND);
	cr:stroke();

	cs:show()
	collectgarbage("collect")
	collectgarbage("step")
end

return M
