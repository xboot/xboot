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

enum bdf_state
{
	BDF_STATE_NONE,
	BDF_STATE_FONT,
	BDF_STATE_PROPERTIES,
	BDF_STATE_CHAR,
};

static x_s8 * bdf_readline(x_s32 fd, x_s8 * buf, x_s32 len)
{
	x_s8 * p = buf;
	x_s8 c;
	x_s32 n;

	while( (--len > 0) && (read(fd, (void *)&c, 1) == 1) )
	{
		*p++ = c;
		if(c == '\n')
			break;
	}

	if(p == buf)
		return NULL;

	*p = 0;
	n = strlen(buf);

	if( n && ((buf[n-1] == '\r') || (buf[n-1] == '\n')) )
		buf[--n] = 0;
	if( n && ((buf[n-1] == '\r') || (buf[n-1] == '\n')) )
		buf[--n] = 0;

	return buf;
}

static x_bool bdf_load(struct font ** font, const char * filename)
{
	struct stat st;
	enum bdf_state state = BDF_STATE_NONE;
	x_s8 line[256];
	x_s32 fd;
	x_s32 chars = 0;

	if(stat(filename, &st) != 0)
		return FALSE;

	if(S_ISDIR(st.st_mode))
		return FALSE;

	fd = open(filename, O_RDONLY, (S_IRUSR|S_IRGRP|S_IROTH));
	if(fd < 0)
		return FALSE;

	if(bdf_readline(fd, line, sizeof(line)) == NULL)
	{
		close(fd);
		return FALSE;
	}

	if(strncmp(line, (const x_s8 *)"STARTFONT", sizeof("STARTFONT") - 1) != 0)
	{
		close(fd);
		return FALSE;
	}

	if(lseek(fd, 0, SEEK_SET) < 0)
	{
		close(fd);
		return FALSE;
	}

	while(bdf_readline(fd, line, sizeof(line)) != NULL)
	{
		switch(state)
		{
		case BDF_STATE_NONE:
			if(strncmp(line, (const x_s8 *)"STARTFONT", sizeof("STARTFONT") - 1) == 0)
			{
				state = BDF_STATE_FONT;
			}
			break;

		case BDF_STATE_FONT:
			if(strncmp(line, (const x_s8 *)"STARTPROPERTIES", sizeof("STARTPROPERTIES") - 1) == 0)
			{
				state = BDF_STATE_PROPERTIES;
			}
			else if(strncmp(line, (const x_s8 *)"STARTCHAR", sizeof("STARTCHAR") - 1) == 0)
			{
				state = BDF_STATE_CHAR;
			}
			else if(strncmp(line, (const x_s8 *)"CHARS", sizeof("CHARS") - 1) == 0)
			{
				if( (sscanf(line, (const x_s8 *)"CHARS %ld", &chars) != 1) || (chars <= 0) )
				{
					close(fd);
					return FALSE;
				}

				if(!font_create(font, "default", chars))
				{
					close(fd);
					return FALSE;
				}
			}
			else if(strncmp(line, (const x_s8 *)"ENDFONT", sizeof("ENDFONT") - 1) == 0)
			{
				state = BDF_STATE_NONE;
				close(fd);
				if(chars <= 0)
					return FALSE;
				return TRUE;
			}
			break;

		case BDF_STATE_PROPERTIES:
			if(strncmp(line, (const x_s8 *)"ENDPROPERTIES", sizeof("ENDPROPERTIES") - 1) == 0)
			{
				state = BDF_STATE_FONT;
			}
			break;

		case BDF_STATE_CHAR:
			if(strncmp(line, (const x_s8 *)"ENDCHAR", sizeof("ENDCHAR") - 1) == 0)
			{
				state = BDF_STATE_FONT;
			}
			break;
		}
	}

	close(fd);
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
