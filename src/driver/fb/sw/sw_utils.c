/*
 * driver/fb/sw/sw_utils.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
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

#include <fb/sw/sw.h>

pixman_format_code_t pixel_format_to_pixman_format_code(enum pixel_format_t format)
{
    pixman_format_code_t ret;

	switch(format)
	{
	case PIXEL_FORMAT_ARGB32:
		ret = PIXMAN_a8r8g8b8;
		break;

	case PIXEL_FORMAT_RGB24:
		ret = PIXMAN_x8r8g8b8;
		break;

	case PIXEL_FORMAT_A8:
		ret = PIXMAN_a8;
		break;

	case PIXEL_FORMAT_A1:
		ret = PIXMAN_a1;
		break;

	case PIXEL_FORMAT_RGB16_565:
		ret = PIXMAN_r5g6b5;
		break;

	case PIXEL_FORMAT_RGB30:
		ret = PIXMAN_x2r10g10b10;
		break;

	default:
		ret = PIXMAN_a8r8g8b8;
		break;
	}

	return ret;
}
