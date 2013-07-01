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

	cr:set_line_width(6);

	cr:rectangle(12, 12, 232, 70);
	cr:new_sub_path(); cr:arc(64, 64, 40, 0, 2*M_PI);
	cr:new_sub_path(); cr:arc_negative(192, 64, 40, 0, -2*M_PI);

	cr:set_fill_rule(cairo.FILL_RULE_EVEN_ODD);
	cr:set_source_rgb(0, 0.7, 0); cr:fill_preserve();
	cr:set_source_rgb(0, 0, 0); cr:stroke();

	cr:translate(0, 128);
	cr:rectangle(12, 12, 232, 70);
	cr:new_sub_path(); cr:arc(64, 64, 40, 0, 2*M_PI);
	cr:new_sub_path(); cr:arc_negative(192, 64, 40, 0, -2*M_PI);

	cr:set_fill_rule(cairo.FILL_RULE_WINDING);
	cr:set_source_rgb(0, 0, 0.9); cr:fill_preserve();
	cr:set_source_rgb(0, 0, 0); cr:stroke();

	cs:show()
	collectgarbage("collect")
	collectgarbage("step")
end

return M
