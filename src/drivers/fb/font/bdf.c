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
#include <ctype.h>
#include <malloc.h>
#include <byteorder.h>
#include <vsprintf.h>
#include <xboot/log.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <xboot/list.h>
#include <fs/fsapi.h>
#include <fb/font.h>

struct bdf_info
{
	/* font name */
	x_s8 name[256];

	/* glyph's pixel and dots per inch */
	x_s32 size;
	x_s32 xres, yres;

	/* font bounding box */
	x_s32 fbbx, fbby, xoff, yoff;

	/* the number of glyphs */
	x_s32 chars;
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

static x_bool get_bdf_info(x_s32 fd, struct bdf_info * info)
{
	x_s8 line[256];

	if(fd < 0)
		return FALSE;

	if(!info)
		return FALSE;

	if(lseek(fd, 0, SEEK_SET) < 0)
		return FALSE;

	if(bdf_readline(fd, line, sizeof(line)) == NULL)
		return FALSE;

	if(strncmp(line, (const x_s8 *)"STARTFONT ", sizeof("STARTFONT ") - 1) != 0)
		return FALSE;

	memset(info, 0, sizeof(struct bdf_info));

	while(bdf_readline(fd, line, sizeof(line)) != NULL)
	{
		if(strncmp(line, (const x_s8 *)"FONT ", sizeof("FONT ") - 1) == 0)
		{
			if(sscanf(line, (const x_s8 *)"FONT %s", info->name) != 1)
				return FALSE;
		}
		else if(strncmp(line, (const x_s8 *)"SIZE ", sizeof("SIZE ") - 1) == 0)
		{
			if(sscanf(line, (const x_s8 *)"SIZE %ld %ld %ld", &info->size, &info->xres, &info->yres) != 3)
				return FALSE;
		}
		else if(strncmp(line, (const x_s8 *)"FONTBOUNDINGBOX ", sizeof("FONTBOUNDINGBOX ") - 1) == 0)
		{
			if(sscanf(line, (const x_s8 *)"FONTBOUNDINGBOX %ld %ld %ld %ld", &info->fbbx, &info->fbby, &info->xoff, &info->yoff) != 4)
				return FALSE;
		}
		else if(strncmp(line, (const x_s8 *)"CHARS ", sizeof("CHARS ") - 1) == 0)
		{
			if( (sscanf(line, (const x_s8 *)"CHARS %ld", &info->chars) != 1) || (info->chars <= 0) )
				return FALSE;
			break;
		}
	}

	if( (info->size > 0) && (info->chars > 0) && (strlen(info->name) > 0) )
		return TRUE;

	return FALSE;
}

static x_bool bdf_add_next_font_glyph(struct font * font, x_s32 fd, struct bdf_info * info)
{
	struct font_glyph * glyph;
	x_s8 line[256];
	x_s8 * pline;
	x_s32 encoding = -1;
	x_s32 w = 0, h = 0;
	x_s32 x = 0, y = 0;
	x_u8 * data;
	x_u8 * pdata;
	x_u8 c;
	x_s32 pitch, len;
	x_s32 i, j;

	if(fd < 0)
		return FALSE;

	if(!font)
		return FALSE;

	if(!info)
		return FALSE;

	while(bdf_readline(fd, line, sizeof(line)) != NULL)
	{
		if(strncmp(line, (const x_s8 *)"ENCODING ", sizeof("ENCODING ") - 1) == 0)
		{
			if( sscanf(line, (const x_s8 *)"ENCODING %ld", &encoding) != 1 )
				return FALSE;
		}
		else if(strncmp(line, (const x_s8 *)"BBX ", sizeof("BBX ") - 1) == 0)
		{
			if(sscanf(line, (const x_s8 *)"BBX %ld %ld %ld %ld", &w, &h, &x, &y) != 4)
				return FALSE;
		}
		else if(strncmp(line, (const x_s8 *)"BITMAP", sizeof("BITMAP") - 1) == 0)
		{
			break;
		}
		else if(strncmp(line, (const x_s8 *)"ENDFONT", sizeof("ENDFONT") - 1) == 0)
		{
			return FALSE;
		}
	}

	if( (encoding == -1) || (w == 0) || (h == 0) )
		return FALSE;

	pitch = (w + 7) / 8;
	len = pitch * h;

	pdata = data = malloc(len);
	if(!data)
		return FALSE;
	memset(data, 0, len);

	for(i = 0, j = 0; i < h; i++)
	{
		if(bdf_readline(fd, line, sizeof(line)) == NULL)
			break;

		for(pline = line; *pline; pline++)
		{
			if(!isxdigit(*pline))
				break;

			if(*pline <= '9')
				c = (*pline - '0') & 0xf;
			else
				c = (tolower(*pline) - 'a' + 10) & 0xf;

			if( (j++) & 0x1 )
			{
				*pdata  = (*pdata & 0xf0) | c;
				pdata++;
			}
			else
			{
				*pdata = (*pdata & 0x0f) | (c << 4);
			}
		}
	}

	glyph = malloc(sizeof(struct font_glyph));
	if(!glyph)
	{
		free(data);
		return FALSE;
	}

	glyph->code = encoding;
	glyph->w = w;
	glyph->h = h;
	glyph->data = data;

	if(!add_font_glyph(font, glyph))
	{
		free(data);
		free(glyph);
		return FALSE;
	}

	return TRUE;
}

static x_bool bdf_load(struct font ** font, const char * filename)
{
	struct bdf_info info;
	struct stat st;
	x_s32 fd;

	if(stat(filename, &st) != 0)
		return FALSE;

	if(S_ISDIR(st.st_mode))
		return FALSE;

	fd = open(filename, O_RDONLY, (S_IRUSR|S_IRGRP|S_IROTH));
	if(fd < 0)
		return FALSE;

	if(!get_bdf_info(fd, &info))
	{
		close(fd);
		return FALSE;
	}

	if(!font_create(font, (const char *)info.name, info.chars))
	{
		close(fd);
		return FALSE;
	}

	while(bdf_add_next_font_glyph(*font, fd, &info));

	close(fd);
	return TRUE;
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
