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
	
	cr:set_line_width(30.0);
	cr:set_line_cap(cairo.LINE_CAP_BUTT);
	cr:move_to(64.0, 50.0); cr:line_to(64.0, 200.0);
	cr:stroke();
	cr:set_line_cap(cairo.LINE_CAP_ROUND);
	cr:move_to(128.0, 50.0); cr:line_to(128.0, 200.0);
	cr:stroke();
	cr:set_line_cap(cairo.LINE_CAP_SQUARE);
	cr:move_to(192.0, 50.0); cr:line_to(192.0, 200.0);
	cr:stroke();

	cr:set_source_rgb(1, 0.2, 0.2);
	cr:set_line_width(2.56);
	cr:move_to(64.0, 50.0); cr:line_to(64.0, 200.0);
	cr:move_to(128.0, 50.0);  cr:line_to(128.0, 200.0);
	cr:move_to(192.0, 50.0); cr:line_to(192.0, 200.0);
	cr:stroke();

	cs:show()
	collectgarbage("collect")
	collectgarbage("step")
end

return M
