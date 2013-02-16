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
	
    cr:arc(128.0, 128.0, 76.8, 0, 2*M_PI);
    cr:clip();
    cr:new_path();

    local image = cairo.image_surface_create_from_png("/romdisk/system/media/image/battery/battery_8.png");
	local w = image:get_width();
	local h = image:get_height();
	cr:scale(256.0/w, 256.0/h);
--	cr:set_source_surface(image, 0, 0);	--BUG(crash when sencond running), PLEASE FIXME
	cr:paint();

	cs:show()
	collectgarbage("collect")
	collectgarbage("step")
end

return M
