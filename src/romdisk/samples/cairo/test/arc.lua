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
	
    local xc = 128.0;
    local yc = 128.0;
    local radius = 100.0;
    local angle1 = 45.0  * (M_PI / 180.0);
    local angle2 = 180.0 * (M_PI / 180.0);

    cr:set_line_width(10.0)
    cr:arc(xc, yc, radius, angle1, angle2);
    cr:stroke();

    cr:set_source_rgba(1, 0.2, 0.2, 0.6);
    cr:set_line_width(6.0);

    cr:arc(xc, yc, 10.0, 0, 2*M_PI);
    cr:fill();

    cr:arc(xc, yc, radius, angle1, angle1);
    cr:line_to(xc, yc);
    cr:arc(xc, yc, radius, angle2, angle2);
    cr:line_to(xc, yc);
    cr:stroke();
	
	cs:show()
	collectgarbage("collect")
	collectgarbage("step")
end

return M
