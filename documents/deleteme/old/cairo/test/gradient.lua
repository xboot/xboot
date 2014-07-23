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

	local pat = cairo.pattern_create_linear(0.0, 0.0,  0.0, 256.0);
	pat:add_color_stop_rgba(1, 0, 0, 0, 1);
	pat:add_color_stop_rgba(0, 1, 1, 1, 1);
	cr:rectangle(0, 0, 256, 256);
	cr:set_source(pat);
	cr:fill(cr);

	pat = cairo.pattern_create_radial(115.2, 102.4, 25.6, 102.4,  102.4, 128.0);
	pat:add_color_stop_rgba(0, 1, 1, 1, 1);
	pat:add_color_stop_rgba(1, 0, 0, 0, 1);
	cr:set_source(pat);
	cr:arc(128.0, 128.0, 76.8, 0, 2 * M_PI);
	cr:fill(cr);

	cs:show()
	collectgarbage("collect")
	collectgarbage("step")
end

return M
