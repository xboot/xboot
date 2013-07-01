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

	local x=25.6
	local y=128.0
	local x1=102.4
	local y1=230.4
	local x2=153.6
	local y2=25.6
	local x3=230.4
	local y3=128.0

	cr:move_to(x, y);
	cr:curve_to(x1, y1, x2, y2, x3, y3);

	cr:set_line_width(10.0);
	cr:stroke();

	cr:set_source_rgba(1, 0.2, 0.2, 0.6);
	cr:set_line_width(6.0);
	cr:move_to(x,y);   cr:line_to(x1,y1);
	cr:move_to(x2,y2); cr:line_to(x3,y3);
	cr:stroke();

	cs:show()
	collectgarbage("collect")
	collectgarbage("step")
end

return M
