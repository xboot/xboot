/*
 * drivers/fb/graphic.c
 *
 *
 * Copyright (c) 2007-2009  jianjun jiang <jerryjianjun@gmail.com>
 * website: http://xboot.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <configs.h>
#include <default.h>
#include <types.h>
#include <xboot.h>
#include <malloc.h>
#include <xboot/chrdev.h>
#include <fb/fb.h>
#include <fb/graphic.h>


/*
 * draw line directly, one pixel.
 */
x_bool draw_line(struct fb * fb, x_u32 x1, x_u32 y1, x_u32 x2, x_u32 y2, x_u32 c)
{
	x_s32 dx, dy;
	x_s32 dx_sym, dy_sym;
	x_s32 dx_x2, dy_x2;
   	x_s32 di;

   	dx = x2 - x1;
   	dy = y2 - y1;

   	if(dx > 0)
   		dx_sym = 1;
   	else if(dx < 0)
   	  	dx_sym = -1;
   	else
   	{
   		if(dy > 0)
   			fb->vline(x1, y1, y2-y1, c);
   		else
   			fb->vline(x1, y2, y1-y2, c);
      	return TRUE;
   	}

	if(dy>0)
   		dy_sym = 1;
   	else if(dy<0)
   		dy_sym = -1;
  	else
  	{
  		if(dx > 0)
  			fb->hline(x1, y1, x2-x1, c);
  		else
  			fb->hline(x2, y1, x1-x2, c);
      	return TRUE;
   	}

   	dx = dx_sym * dx;
   	dy = dy_sym * dy;
   	dx_x2 = dx*2;
   	dy_x2 = dy*2;

   	if(dx>=dy)
   	{
   		di = dy_x2 - dx;
      	while(x1 != x2)
      	{
      		fb->set_pixel(x1, y1, c);
         	x1 += dx_sym;
         	if(di < 0)
         	{
         		di += dy_x2;
         	}
         	else
         	{
         		di += dy_x2 - dx_x2;
            	y1 += dy_sym;
         	}
      	}
      	fb->set_pixel(x1, y1, c);
   	}
   	else
   	{
   		di = dx_x2 - dy;
      	while(y1 != y2)
      	{
      		fb->set_pixel(x1, y1, c);
         	y1 += dy_sym;
         	if(di < 0)
         	{
         		di += dx_x2;
         	}
         	else
         	{
         		di += dx_x2 - dy_x2;
            	x1 += dx_sym;
         	}
      	}
      	fb->set_pixel(x1, y1, c);
   	}

   	return TRUE;
}

/*
 * draw line with width.
 */
x_bool draw_line_ex(struct fb * fb, x_u32 x1, x_u32 y1, x_u32 x2, x_u32 y2, x_u32 w, x_u32 c)
{
	x_s32 dx, dy;
	x_s32 dx_sym, dy_sym;
	x_s32 dx_x2, dy_x2;
	x_s32 di;
	x_s32 wx, wy;
	x_s32 draw_a, draw_b;

	if(!fb || w==0)
		return FALSE;

   	dx = x2-x1;
   	dy = y2-y1;
   	wx = w/2;
   	wy = w - wx - 1;

   	if(dx > 0)
   		dx_sym = 1;
   	else if(dx < 0)
   	  	dx_sym = -1;
   	else
   	{
		wx = x1 - wx;
        if(wx < 0)
        	wx = 0;
        wy = x1 + wy;

        while(1)
        {
        	x1 = wx;
       		if(dy > 0)
       			fb->vline(x1, y1, y2-y1, c);
       		else
       			fb->vline(x1, y2, y1-y2, c);
            if(wx >= wy)
            	break;
            wx++;
        }
        return TRUE;
   	}

	if(dy > 0)
   		dy_sym = 1;
   	else if(dy < 0)
   		dy_sym = -1;
  	else
  	{
  		wx = y1 - wx;
        if(wx < 0)
        	wx = 0;
        wy = y1 + wy;

        while(1)
        {
        	y1 = wx;
      		if(dx > 0)
      			fb->hline(x1, y1, x2-x1, c);
      		else
      			fb->hline(x2, y1, x1-x2, c);
            if(wx >= wy)
            	break;
            wx++;
         }
      	 return TRUE;
   	}

   	dx = dx_sym * dx;
   	dy = dy_sym * dy;

   	dx_x2 = dx*2;
   	dy_x2 = dy*2;

   	if(dx >= dy)
   	{
   		di = dy_x2 - dx;
      	while(x1 != x2)
      	{
         	draw_a = y1 - wx;
         	if(draw_a < 0)
         		draw_a = 0;
         	draw_b = y1 + wy;
         	fb->vline(x1, draw_a, draw_b-draw_a, c);
            x1 += dx_sym;
         	if(di<0)
         	{
         		di += dy_x2;
         	}
         	else
         	{
         		di += dy_x2 - dx_x2;
            	y1 += dy_sym;
         	}
      	}
      	draw_a = y1 - wx;
      	if(draw_a < 0)
      		draw_a = 0;
      	draw_b = y1 + wy;
      	fb->vline(x1, draw_a, draw_b-draw_a, c);
   	}
   	else
   	{
   		di = dx_x2 - dy;
      	while(y1 != y2)
      	{
      		draw_a = x1 - wx;
         	if(draw_a < 0)
         		draw_a = 0;
         	draw_b = x1 + wy;
         	fb->hline(draw_a, y1, draw_b-draw_a, c);
            y1 += dy_sym;
         	if(di < 0)
         	{
         		di += dx_x2;
         	}
         	else
         	{
         		di += dx_x2 - dy_x2;
         		x1 += dx_sym;
         	}
      	}
      	draw_a = x1 - wx;
      	if(draw_a < 0)
      		draw_a = 0;
      	draw_b = x1 + wy;
      	fb->hline(draw_a, y1, draw_b-draw_a, c);
   	}
   	return TRUE;
}

/*
 * draw rect.
 */
x_bool draw_rect(struct fb * fb, struct rect * rect, x_u32 c)
{
	x_u32 x0, y0, x1, y1;

	x0 = MIN(rect->x0, rect->x1);
	y0 = MIN(rect->y0, rect->y1);
	x1 = MAX(rect->x0, rect->x1);
	y1 = MAX(rect->y0, rect->y1);

	fb->hline(x0, y0, x1-x0, c);
	fb->hline(x0, y1, x1-x0, c);
	fb->vline(x0, y0, y1-y0, c);
	fb->vline(x1, y0, y1-y0, c);

	return TRUE;
}
