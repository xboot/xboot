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

#include <xboot.h>
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
	s8_t name[256];

	/* glyph's pixel and dots per inch */
	s32_t size;
	s32_t xres, yres;

	/* font bounding box */
	s32_t fbbx, fbby, xoff, yoff;

	/* the number of glyphs */
	s32_t chars;
};

static s8_t * bdf_readline(s32_t fd, s8_t * buf, s32_t len)
{
	s8_t * p = buf;
	s8_t c;
	s32_t n;

	while( (--len > 0) && (read(fd, (void *)&c, 1) == 1) )
	{
		*p++ = c;
		if(c == '\n')
			break;
	}

	if(p == buf)
		return NULL;

	*p = 0;
	n = strlen((const char *)buf);

	if( n && ((buf[n-1] == '\r') || (buf[n-1] == '\n')) )
		buf[--n] = 0;
	if( n && ((buf[n-1] == '\r') || (buf[n-1] == '\n')) )
		buf[--n] = 0;

	return buf;
}

static bool_t get_bdf_info(s32_t fd, struct bdf_info * info)
{
	s8_t line[256];

	if(fd < 0)
		return FALSE;

	if(!info)
		return FALSE;

	if(lseek(fd, 0, SEEK_SET) < 0)
		return FALSE;

	if(bdf_readline(fd, line, sizeof(line)) == NULL)
		return FALSE;

	if(strncmp((const char *)line, "STARTFONT ", sizeof("STARTFONT ") - 1) != 0)
		return FALSE;

	memset(info, 0, sizeof(struct bdf_info));

	while(bdf_readline(fd, line, sizeof(line)) != NULL)
	{
		if(strncmp((const char *)line, "FONT ", sizeof("FONT ") - 1) == 0)
		{
			if(sscanf(line, (const s8_t *)"FONT %s", info->name) != 1)
				return FALSE;
		}
		else if(strncmp((const char *)line, "SIZE ", sizeof("SIZE ") - 1) == 0)
		{
			if(sscanf(line, (const s8_t *)"SIZE %ld %ld %ld", &info->size, &info->xres, &info->yres) != 3)
				return FALSE;
		}
		else if(strncmp((const char *)line, "FONTBOUNDINGBOX ", sizeof("FONTBOUNDINGBOX ") - 1) == 0)
		{
			if(sscanf(line, (const s8_t *)"FONTBOUNDINGBOX %ld %ld %ld %ld", &info->fbbx, &info->fbby, &info->xoff, &info->yoff) != 4)
				return FALSE;
		}
		else if(strncmp((const char *)line, "CHARS ", sizeof("CHARS ") - 1) == 0)
		{
			if( (sscanf(line, (const s8_t *)"CHARS %ld", &info->chars) != 1) || (info->chars <= 0) )
				return FALSE;
			break;
		}
	}

	if( (info->size > 0) && (info->chars > 0) && (strlen((const char *)info->name) > 0) )
		return TRUE;

	return FALSE;
}

static bool_t bdf_add_next_font_glyph(struct font * font, s32_t fd, struct bdf_info * info)
{
	struct font_glyph * glyph;
	s8_t line[256];
	s8_t * pline;
	s32_t encoding = -1;
	s32_t w = 0, h = 0;
	s32_t x = 0, y = 0;
	u8_t * data;
	u8_t * pdata;
	u8_t c;
	s32_t len;
	s32_t i, j;

	if(fd < 0)
		return FALSE;

	if(!font)
		return FALSE;

	if(!info)
		return FALSE;

	while(bdf_readline(fd, line, sizeof(line)) != NULL)
	{
		if(strncmp((const char *)line, "ENCODING ", sizeof("ENCODING ") - 1) == 0)
		{
			if( sscanf(line, (const s8_t *)"ENCODING %ld", &encoding) != 1 )
				return FALSE;
		}
		else if(strncmp((const char *)line, "BBX ", sizeof("BBX ") - 1) == 0)
		{
			if(sscanf(line, (const s8_t *)"BBX %ld %ld %ld %ld", &w, &h, &x, &y) != 4)
				return FALSE;
		}
		else if(strncmp((const char *)line, "BITMAP", sizeof("BITMAP") - 1) == 0)
		{
			break;
		}
		else if(strncmp((const char *)line, "ENDFONT", sizeof("ENDFONT") - 1) == 0)
		{
			return FALSE;
		}
	}

	if( (encoding == -1) || (w == 0) || (h == 0) )
		return FALSE;

	len = ((w + 7) / 8) * h;
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

static bool_t bdf_load(struct font ** font, const char * filename)
{
	struct bdf_info info;
	struct stat st;
	s32_t fd;

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

