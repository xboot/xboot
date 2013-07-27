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

	cr:move_to(128.0, 25.6);
	cr:line_to(230.4, 230.4);
	cr:rel_line_to(-102.4, 0.0);
	cr:curve_to(51.2, 230.4, 51.2, 128.0, 128.0, 128.0);
	cr:close_path();

	cr:move_to(64.0, 25.6);
	cr:rel_line_to(51.2, 51.2);
	cr:rel_line_to(-51.2, 51.2);
	cr:rel_line_to(-51.2, -51.2);
	cr:close_path();

	cr:set_line_width(10.0);
	cr:set_source_rgb(0, 0, 1);
	cr:fill_preserve();
	cr:set_source_rgb(0, 0, 0);
	cr:stroke();

	cs:show()
	collectgarbage("collect")
	collectgarbage("step")
end

return M
