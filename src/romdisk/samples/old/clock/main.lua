local cairo = require "org.xboot.cairo"

local M_PI = math.pi
local sin = math.sin
local cos = math.cos

local tm_sec = 0
local tm_min = 0
local tm_hour = 0
	
function draw(width, height)
	local cs = cairo.image_surface_create(cairo.FORMAT_ARGB32, width, height)
	local cr = cairo.create(cs)
	
	tm_sec = tm_sec + 1;
    if(tm_sec >= 60) then
     	tm_sec = 0
     	tm_min = tm_min + 1
     	if (tm_min >= 60) then
     		tm_min = 0
     		tm_hour = tm_hour + 1;
     		if(tm_hour >= 24) then
     			tm_hour = 0;
     		end
     	end
    end

	local seconds = tm_sec * M_PI / 30;
	local minutes = tm_min * M_PI / 30;
	local hours = tm_hour * M_PI / 6;

	cr:scale(width, height);
	cr:set_source_rgb(1* tm_sec/60, 1*tm_min/60, 1*tm_hour/24);
	cr:paint();

	cr:set_line_cap(cairo.LINE_CAP_ROUND);
	cr:set_line_width(0.1);

	cr:set_source_rgb(0, 0, 0);
	cr:translate(0.5, 0.5);
	cr:arc(0, 0, 0.4, 0, M_PI * 2);
	cr:stroke();

	cr:set_source_rgba(1, 1, 1, 0.6);
	cr:arc(sin(seconds) * 0.4, -cos (seconds) * 0.4,	0.05, 0, M_PI * 2);
	cr:fill();

	cr:set_source_rgba(0.2, 0.2, 1, 0.6);
	cr:move_to(0, 0);
	cr:line_to(sin(minutes) * 0.4, -cos (minutes) * 0.4);
	cr:stroke();

	cr:move_to(0, 0);
	cr:line_to(sin(hours) * 0.2, -cos (hours) * 0.2);
	cr:stroke();
	
	cs:show()
end

local function main()
	local width = 100
	local height = 100
	local flag_w = true;
	local flag_h = true;
	
	while true do
		if(flag_w) then 
			width = width + 1
			if(width >= 400) then
				flag_w = false
			end
		else
			width = width -1
			if(width < 10) then
				flag_w = true
			end
		end
		
		if(flag_h) then 
			height = height + 1
			if(height >= 200) then
				flag_h = false
			end
		else
			height = height -1
			if(height < 10) then
				flag_h = true
			end
		end
			
		draw(width, height);
		collectgarbage("collect")
		collectgarbage("step")
	end
end

main()
