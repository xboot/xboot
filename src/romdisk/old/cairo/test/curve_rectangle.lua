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

	local x0      = 25.6
	local y0      = 25.6
	local rect_width  = 204.8
	local rect_height = 204.8
	local radius = 102.4

	local x1,y1;

	x1=x0+rect_width;
	y1=y0+rect_height;
	if(rect_width==0 or rect_height==0) then
		return;
	end
	if(rect_width/2<radius) then
		if(rect_height/2<radius) then
			cr:move_to(x0,(y0 + y1)/2);
			cr:curve_to(x0 ,y0, x0, y0,(x0 + x1)/2, y0);
			cr:curve_to(x1, y0, x1, y0, x1,(y0 + y1)/2);
			cr:curve_to(x1, y1, x1, y1,(x1 + x0)/2, y1);
			cr:curve_to(x0, y1, x0, y1, x0,(y0 + y1)/2);
		else
			cr:move_to(x0, y0 + radius);
			cr:curve_to(x0 ,y0, x0, y0,(x0 + x1)/2, y0);
			cr:curve_to(x1, y0, x1, y0, x1, y0 + radius);
			cr:line_to(x1 , y1 - radius);
			cr:curve_to(x1, y1, x1, y1,(x1 + x0)/2, y1);
			cr:curve_to(x0, y1, x0, y1, x0, y1- radius);
		end
	else
		if(rect_height/2<radius) then
			cr:move_to(x0,(y0 + y1)/2);
			cr:curve_to(x0 , y0, x0 , y0, x0 + radius, y0);
			cr:line_to(x1 - radius, y0);
			cr:curve_to(x1, y0, x1, y0, x1,(y0 + y1)/2);
			cr:curve_to(x1, y1, x1, y1, x1 - radius, y1);
			cr:line_to(x0 + radius, y1);
			cr:curve_to(x0, y1, x0, y1, x0,(y0 + y1)/2);
		else
			cr:move_to(x0, y0 + radius);
			cr:curve_to(x0 , y0, x0 , y0, x0 + radius, y0);
			cr:line_to(x1 - radius, y0);
			cr:curve_to(x1, y0, x1, y0, x1, y0 + radius);
			cr:line_to(x1 , y1 - radius);
			cr:curve_to(x1, y1, x1, y1, x1 - radius, y1);
			cr:line_to(x0 + radius, y1);
			cr:curve_to(x0, y1, x0, y1, x0, y1- radius);
		end
	end
	cr:close_path();

	cr:set_source_rgb(0.5, 0.5, 1);
	cr:fill_preserve();
	cr:set_source_rgba(0.5, 0, 0, 0.5);
	cr:set_line_width(10.0);
	cr:stroke();

	cs:show()
	collectgarbage("collect")
	collectgarbage("step")
end

return M
