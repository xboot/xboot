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
	
	cr:set_line_width(40.96);
	cr:move_to(76.8, 84.48);
	cr:rel_line_to(51.2, -51.2);
	cr:rel_line_to(51.2, 51.2);
	cr:set_line_join(cairo.LINE_JOIN_MITER);
	cr:stroke();

	cr:move_to(76.8, 161.28);
	cr:rel_line_to(51.2, -51.2);
	cr:rel_line_to(51.2, 51.2);
	cr:set_line_join(cairo.LINE_JOIN_BEVEL);
	cr:stroke();

	cr:move_to(76.8, 238.08);
	cr:rel_line_to(51.2, -51.2);
	cr:rel_line_to(51.2, 51.2);
	cr:set_line_join(cairo.LINE_JOIN_ROUND);
	cr:stroke();

	cs:show()
	collectgarbage("collect")
	collectgarbage("step")
end

return M
