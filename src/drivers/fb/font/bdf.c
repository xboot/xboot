/*
 * drivers/fb/font/bdf.c
 *
 * Copyright (c) 2007-2010  jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
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
#include <malloc.h>
#include <byteorder.h>
#include <vsprintf.h>
#include <xboot/log.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <xboot/list.h>
#include <fs/fsapi.h>
#include <fb/font.h>

static x_bool bdf_load(struct font ** font, const char * filename)
{
	return FALSE;
}

static struct font_reader font_reader_bdf = {
	.extension		= ".bdf",
	.load			= bdf_load,
};

static __init void font_reader_bdf_init(void)
{
	if(!register_font_reader(&font_reader_bdf))
		LOG_E("register 'bdf' font reader fail");
}

static __exit void font_reader_bdf_exit(void)
{
	if(!unregister_font_reader(&font_reader_bdf))
		LOG_E("unregister 'bdf' font reader fail");
}

module_init(font_reader_bdf_init, LEVEL_POSTCORE);
module_exit(font_reader_bdf_exit, LEVEL_POSTCORE);
