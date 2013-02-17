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
	
	local image = cairo.image_surface_create_from_png("/romdisk/system/media/image/battery/battery_8.png");
	local w = image:get_width()
	local h = image:get_height()

	local pattern = cairo.pattern_create_for_surface(image);
	pattern:set_extend(cairo.EXTEND_REPEAT);

	cr:translate(128.0, 128.0);
	cr:rotate(M_PI / 4);
	cr:scale(1 / sqrt (2), 1 / sqrt (2));
	cr:translate(-128.0, -128.0);

	local matrix = cairo.matrix_create()
	matrix:init_scale(w/256.0 * 5.0, h/256.0 * 5.0);
	pattern:set_matrix(matrix);

	cr:set_source(pattern);

	cr:rectangle (0, 0, 256.0, 256.0);
	cr:fill();

	cs:show()
	collectgarbage("collect")
	collectgarbage("step")
end

return M
