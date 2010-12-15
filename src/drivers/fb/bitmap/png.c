/*
 * drivers/fb/bitmap/png.c
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
#include <fb/bitmap.h>

#define DEFLATE_HCLEN_BASE			(4)
#define DEFLATE_HCLEN_MAX			(19)
#define DEFLATE_HLIT_BASE			(257)
#define DEFLATE_HLIT_MAX			(288)
#define DEFLATE_HDIST_BASE			(1)
#define DEFLATE_HDIST_MAX			(30)

#define DEFLATE_HUFF_LEN			(16)
#define WINDOW_SIZE					(0x8000)

enum
{
	PNG_COLOR_TYPE_GRAY				= 0x0,
	PNG_COLOR_TYPE_PALETTE			= 0x3,
	PNG_COLOR_TYPE_RGB				= 0x2,
	PNG_COLOR_TYPE_RGBA				= 0x6,
	PNG_COLOR_TYPE_GRAYA			= 0x4,
};

enum
{
	PNG_CHUNK_TYPE_IHDR				= 0x49484452,
	PNG_CHUNK_TYPE_IDAT				= 0x49444154,
	PNG_CHUNK_TYPE_IEND				= 0x49454e44,
};

enum
{
	PNG_FILTER_VALUE_NONE			= 0x0,
	PNG_FILTER_VALUE_SUB			= 0x1,
	PNG_FILTER_VALUE_UP				= 0x2,
	PNG_FILTER_VALUE_AVG			= 0x3,
	PNG_FILTER_VALUE_PAETH			= 0x4,
	PNG_FILTER_VALUE_LAST			= 0x5,
};

enum
{
	PNG_BLOCK_TYPE_INFLATE_STORED	= 0x0,
	PNG_BLOCK_TYPE_INFLATE_FIXED	= 0x1,
	PNG_BLOCK_TYPE_INFLATE_DYNAMIC	= 0x2,
};

struct huff_table
{
	x_s32 * values, * maxval, * offset;
	x_s32 num_values, max_length;
};

struct png_data
{
	x_s32 file;
	x_u32 file_offset;
	struct bitmap ** bitmap;

	x_s32 bit_count, bit_save;

	x_u32 next_offset;

	x_s32 image_width, image_height, bpp, is_16bit, raw_bytes;
	x_u8 * image_data;

	x_s32 inside_idat, idat_remain;

	x_s32 code_values[DEFLATE_HLIT_MAX];
	x_s32 code_maxval[DEFLATE_HUFF_LEN];
	x_s32 code_offset[DEFLATE_HUFF_LEN];

	x_s32 dist_values[DEFLATE_HDIST_MAX];
	x_s32 dist_maxval[DEFLATE_HUFF_LEN];
	x_s32 dist_offset[DEFLATE_HUFF_LEN];

	struct huff_table code_table;
	struct huff_table dist_table;

	x_u8 slide[WINDOW_SIZE];
	x_s32 wp;

	x_u8 * cur_rgb;

	x_s32 cur_column, cur_filter, first_line;
};

/* png magic number */
static const x_u8 png_magic[8] = {
	0x89, 0x50, 0x4e, 0x47, 0xd, 0xa, 0x1a, 0x0a
};

/* order of the bit length code lengths */
static const x_u8 bitorder[] = {
	16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
};

/* copy lengths for literal codes 257..285 */
static const x_s32 cplens[] = {
	3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
	35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0, 0
};

/* extra bits for literal codes 257..285 */
static const x_u8 cplext[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
	3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 99, 99
};

/* copy offsets for distance codes 0..29 */
static const x_s32 cpdist[] = {
	1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
	257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145,
	8193, 12289, 16385, 24577
};

/* extra bits for distance codes */
static const x_u8 cpdext[] = {
	0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
	7, 7, 8, 8, 9, 9, 10, 10, 11, 11,
	12, 12, 13, 13
};

static x_u32 png_get_dword(struct png_data * data)
{
	x_u32 r = 0;

	data->file_offset += read(data->file, &r, sizeof(x_u32));
	return be32_to_cpu(r);
}

static x_u8 png_get_byte(struct png_data * data)
{
	x_u8 r;
	x_u32 len, type;

	if((data->inside_idat) && (data->idat_remain == 0))
	{
		do
		{
			png_get_dword(data);

			if(data->file_offset != data->next_offset)
				return 0;

			len = png_get_dword(data);
			type = png_get_dword(data);
			if(type != PNG_CHUNK_TYPE_IDAT)
				return 0;

			data->next_offset = data->file_offset + len + 4;
		}while(len == 0);
		data->idat_remain = len;
	}

	r = 0;
	data->file_offset += read(data->file, &r, sizeof(x_u8));

	if(data->inside_idat)
		data->idat_remain--;

	return r;
}

static x_s32 png_get_bits(struct png_data * data, x_s32 num)
{
	x_s32 code, shift;
	x_s32 n;

	if(data->bit_count == 0)
	{
		data->bit_save = png_get_byte(data);
		data->bit_count = 8;
	}

	code = 0;
	shift = 0;
	while(1)
	{
		n = data->bit_count;
		if(n > num)
			n = num;

		code += (x_s32)(data->bit_save & ((1 << n) - 1)) << shift;
		num -= n;
		if(!num)
		{
			data->bit_count -= n;
			data->bit_save >>= n;
			break;
		}

		shift += n;
		data->bit_save = png_get_byte(data);
		data->bit_count = 8;
	}

	return code;
}

static void png_init_huff_table(struct huff_table * ht, x_s32 cur_maxlen, x_s32 * cur_values, x_s32 * cur_maxval, x_s32 * cur_offset)
{
	ht->values = cur_values;
	ht->maxval = cur_maxval;
	ht->offset = cur_offset;
	ht->num_values = 0;
	ht->max_length = cur_maxlen;
	memset(cur_maxval, 0, sizeof(x_s32) * cur_maxlen);
}

static void png_insert_huff_item(struct huff_table * ht, x_s32 code, x_s32 len)
{
	x_s32 i, n;

	if(len == 0)
		return;

	if(len > ht->max_length)
		return;

	n = 0;
	for(i = len; i < ht->max_length; i++)
		n += ht->maxval[i];

	for(i = 0; i < n; i++)
		ht->values[ht->num_values - i] = ht->values[ht->num_values - i - 1];

	ht->values[ht->num_values - n] = code;
	ht->num_values++;
	ht->maxval[len - 1]++;
}

static void png_build_huff_table(struct huff_table * ht)
{
	x_s32 base, ofs, i;

	base = 0;
	ofs = 0;
	for(i = 0; i < ht->max_length; i++)
	{
		base += ht->maxval[i];
		ofs += ht->maxval[i];

		ht->maxval[i] = base;
		ht->offset[i] = ofs - base;

		base <<= 1;
	}
}

static x_s32 png_get_huff_code(struct png_data * data, struct huff_table * ht)
{
	x_s32 code, i;

	code = 0;
	for(i = 0; i < ht->max_length; i++)
	{
		code = (code << 1) + png_get_bits(data, 1);
		if(code < ht->maxval[i])
			return ht->values[code + ht->offset[i]];
	}

	return 0;
}

static x_bool png_init_fixed_block(struct png_data * data)
{
	x_s32 i;

	png_init_huff_table(&data->code_table, DEFLATE_HUFF_LEN, data->code_values, data->code_maxval, data->code_offset);

	for(i = 0; i < 144; i++)
		png_insert_huff_item(&data->code_table, i, 8);

	for(; i < 256; i++)
		png_insert_huff_item(&data->code_table, i, 9);

	for(; i < 280; i++)
		png_insert_huff_item(&data->code_table, i, 7);

	for(; i < DEFLATE_HLIT_MAX; i++)
		png_insert_huff_item(&data->code_table, i, 8);

	png_build_huff_table(&data->code_table);

	png_init_huff_table(&data->dist_table, DEFLATE_HUFF_LEN, data->dist_values, data->dist_maxval, data->dist_offset);

	for(i = 0; i < DEFLATE_HDIST_MAX; i++)
		png_insert_huff_item(&data->dist_table, i, 5);

	png_build_huff_table(&data->dist_table);

	return TRUE;
}

static x_bool png_init_dynamic_block(struct png_data * data)
{
	x_s32 nl, nd, nb, i, prev;
	struct huff_table cl;
	x_s32 cl_values[sizeof(bitorder)];
	x_s32 cl_maxval[8];
	x_s32 cl_offset[8];
	x_u8 lens[DEFLATE_HCLEN_MAX];
	x_s32 n, code, c;
	struct huff_table *ht;

	nl = DEFLATE_HLIT_BASE + png_get_bits(data, 5);
	nd = DEFLATE_HDIST_BASE + png_get_bits(data, 5);
	nb = DEFLATE_HCLEN_BASE + png_get_bits(data, 4);

	if((nl > DEFLATE_HLIT_MAX) || (nd > DEFLATE_HDIST_MAX) || (nb > DEFLATE_HCLEN_MAX))
	  return FALSE;

	png_init_huff_table(&cl, 8, cl_values, cl_maxval, cl_offset);

	for(i = 0; i < nb; i++)
		lens[bitorder[i]] = png_get_bits(data, 3);

	for(; i < DEFLATE_HCLEN_MAX; i++)
		lens[bitorder[i]] = 0;

	for(i = 0; i < DEFLATE_HCLEN_MAX; i++)
		png_insert_huff_item(&cl, i, lens[i]);

	png_build_huff_table(&cl);

	png_init_huff_table(&data->code_table, DEFLATE_HUFF_LEN, data->code_values, data->code_maxval, data->code_offset);

	png_init_huff_table(&data->dist_table, DEFLATE_HUFF_LEN, data->dist_values, data->dist_maxval, data->dist_offset);

	prev = 0;
	for(i = 0; i < nl + nd; i++)
	{
		if(i < nl)
		{
			ht = &data->code_table;
			code = i;
		}
		else
		{
			ht = &data->dist_table;
			code = i - nl;
		}

		n = png_get_huff_code(data, &cl);
		if(n < 16)
		{
			png_insert_huff_item(ht, code, n);
			prev = n;
		}
		else if(n == 16)
		{
			c = 3 + png_get_bits (data, 2);
			while(c > 0)
			{
				png_insert_huff_item(ht, code++, prev);
				i++;
				c--;
			}
			i--;
		}
		else if(n == 17)
			i += 3 + png_get_bits(data, 3) - 1;
		else
			i += 11 + png_get_bits(data, 7) - 1;
	}

	png_build_huff_table(&data->code_table);
	png_build_huff_table(&data->dist_table);

	return TRUE;
}

static x_bool png_output_byte(struct png_data * data, x_u8 n)
{
	x_s32 row_bytes;
	x_s32 i;
	x_s32 a, b, c, pa, pb, pc;
	x_u8 * blank_line;
	x_u8 * cur;
	x_u8 * left;
	x_u8 * up;
	x_u8 * upper_left;

	if(--data->raw_bytes < 0)
		return FALSE;

	if(data->cur_column == 0)
	{
		if(n >= PNG_FILTER_VALUE_LAST)
			return FALSE;

		data->cur_filter = n;
	}
	else
		*(data->cur_rgb++) = n;

	data->cur_column++;
	row_bytes = data->image_width * data->bpp;
	if(data->cur_column == row_bytes + 1)
	{
		blank_line = NULL;
		cur = data->cur_rgb - row_bytes;
		left = cur;

		if(data->first_line)
		{
			blank_line = malloc(row_bytes);
			if(blank_line == NULL)
				return FALSE;

			up = blank_line;
		}
		else
			up = cur - row_bytes;

		switch(data->cur_filter)
		{
		case PNG_FILTER_VALUE_SUB:
			cur += data->bpp;
			for(i = data->bpp; i < row_bytes; i++, cur++, left++)
				*cur += *left;
			break;

		case PNG_FILTER_VALUE_UP:
			for(i = 0; i < row_bytes; i++, cur++, up++)
				*cur += *up;
			break;

		case PNG_FILTER_VALUE_AVG:
			for(i = 0; i < data->bpp; i++, cur++, up++)
				*cur += *up >> 1;
			for(; i < row_bytes; i++, cur++, up++, left++)
				*cur += ((int) *up + (int) *left) >> 1;
			break;

		case PNG_FILTER_VALUE_PAETH:
			upper_left = up;

			for(i = 0; i < data->bpp; i++, cur++, up++)
				*cur += *up;

			for(; i < row_bytes; i++, cur++, up++, left++, upper_left++)
			{
				a = *left;
				b = *up;
				c = *upper_left;

				pa = b - c;
				pb = a - c;
				pc = pa + pb;

				if (pa < 0)
					pa = -pa;

				if (pb < 0)
					pb = -pb;

				if (pc < 0)
					pc = -pc;

				*cur += ((pa <= pb) && (pa <= pc)) ? a : (pb <= pc) ? b : c;
			}
			break;
		}

		if(blank_line)
			free(blank_line);

		data->cur_column = 0;
		data->first_line = 0;
	}

	return TRUE;
}

static x_bool png_read_dynamic_block(struct png_data * data)
{
	x_s32 n;
	x_s32 len, dist, pos;

	while(1)
	{
		n = png_get_huff_code(data, &data->code_table);
		if(n < 256)
		{
			data->slide[data->wp] = n;
			if(!png_output_byte(data, n))
				return FALSE;

			data->wp++;
			if(data->wp >= WINDOW_SIZE)
				data->wp = 0;
		}
		else if(n == 256)
			break;
		else
		{
			n -= 257;
			len = cplens[n];
			if(cplext[n])
				len += png_get_bits(data, cplext[n]);

			n = png_get_huff_code(data, &data->dist_table);
			dist = cpdist[n];
			if(cpdext[n])
				dist += png_get_bits(data, cpdext[n]);

			pos = data->wp - dist;
			if(pos < 0)
				pos += WINDOW_SIZE;

			while (len > 0)
			{
				data->slide[data->wp] = data->slide[pos];
				if(!png_output_byte(data, data->slide[data->wp]))
					return FALSE;

				data->wp++;
				if(data->wp >= WINDOW_SIZE)
					data->wp = 0;

				pos++;
				if(pos >= WINDOW_SIZE)
					pos = 0;

				len--;
			}
		}
	}

	return TRUE;
}

static x_bool png_decode_image_header(struct png_data * data)
{
	x_s32 color_type;
	x_s32 color_bits;

	data->image_width = png_get_dword(data);
	data->image_height = png_get_dword(data);

	if((!data->image_height) || (!data->image_width))
		return FALSE;

	color_bits = png_get_byte(data);
	if((color_bits != 8) && (color_bits != 16))
		return FALSE;
	data->is_16bit = (color_bits == 16);

	color_type = png_get_byte(data);
	if(color_type == PNG_COLOR_TYPE_RGB)
	{
		if(!bitmap_create(data->bitmap, data->image_width, data->image_height, BITMAP_FORMAT_RGB_888))
			return FALSE;
		data->bpp = 3;
	}
	else if(color_type == PNG_COLOR_TYPE_RGBA)
	{
		if(!bitmap_create(data->bitmap, data->image_width, data->image_height, BITMAP_FORMAT_RGBA_8888))
			return FALSE;
		data->bpp = 4;
	}
	else
		return FALSE;

	if(data->is_16bit)
	{
		data->bpp <<= 1;

		data->image_data = malloc(data->image_height * data->image_width * data->bpp);
		if(!data->image_data)
			return FALSE;

		data->cur_rgb = data->image_data;
	}
	else
	{
		data->image_data = 0;
		data->cur_rgb = (*data->bitmap)->data;
	}

	data->raw_bytes = data->image_height * (data->image_width + 1) * data->bpp;

	data->cur_column = 0;
	data->first_line = 1;

	if(png_get_byte(data) != 0)
		return FALSE;

	if(png_get_byte(data) != 0)
		return FALSE;

	if(png_get_byte(data) != 0)
		return FALSE;

	/* skip crc checksum */
	png_get_dword(data);

	return TRUE;
}

static x_bool png_decode_image_data(struct png_data * data)
{
	x_u8 cmf, flg;
	x_s32 final;
	x_s32 block_type;
	x_u16 i, len;

	cmf = png_get_byte(data);
	flg = png_get_byte(data);

	if((cmf & 0xF) != 0x8)
		return FALSE;

	if(flg & 0x20)
		return FALSE;

	do
	{
		final = png_get_bits(data, 1);
		block_type = png_get_bits(data, 2);

		switch(block_type)
		{
		case PNG_BLOCK_TYPE_INFLATE_STORED:
			data->bit_count = 0;
			len = png_get_byte(data);
			len += ((x_u16)png_get_byte(data)) << 8;

			png_get_byte(data);
			png_get_byte(data);

			for(i = 0; i < len; i++)
			{
				if(!png_output_byte(data, png_get_byte(data)))
					return FALSE;
			}
			break;

		case PNG_BLOCK_TYPE_INFLATE_FIXED:
			if(!png_init_fixed_block(data))
				return FALSE;
			if(!png_read_dynamic_block(data))
				return FALSE;
			break;

		case PNG_BLOCK_TYPE_INFLATE_DYNAMIC:
			if(!png_init_dynamic_block(data))
				return FALSE;
			if(!png_read_dynamic_block (data))
				return FALSE;
			break;

		default:
			return FALSE;
		}
	} while((!final));

	/* skip adler checksum */
	png_get_dword(data);

	/* skip crc checksum */
	png_get_dword(data);

	return TRUE;
}

static void png_convert_image(struct png_data * data)
{
	x_s32 i;
	x_u8 *d1, *d2;

	d1 = (*data->bitmap)->data;
	d2 = data->image_data + 1;

	/* only copy the upper 8 bit */
	for(i = 0; i < (data->image_width * data->image_height * data->bpp >> 1); i++, d1++, d2+=2)
		*d1 = *d2;
}

static x_bool png_decode(struct png_data * data)
{
	x_u8 magic[8];
	x_u32 len, type;

	if(read(data->file, &magic[0], 8) != 8)
		return FALSE;
	if(memcmp(magic, png_magic, sizeof(png_magic)))
		return FALSE;
	data->file_offset += 8;

	while(1)
	{
		len = png_get_dword(data);
		type = png_get_dword(data);
		data->next_offset = data->file_offset + len + 4;

		switch(type)
		{
		case PNG_CHUNK_TYPE_IHDR:
			if(!png_decode_image_header(data))
				return FALSE;
			break;

		case PNG_CHUNK_TYPE_IDAT:
			data->inside_idat = 1;
			data->idat_remain = len;
			data->bit_count = 0;
			if(!png_decode_image_data(data))
				return FALSE;
			data->inside_idat = 0;
			break;

		case PNG_CHUNK_TYPE_IEND:
			if(data->is_16bit)
				png_convert_image(data);
			return TRUE;

		default:
			if(lseek(data->file, data->file_offset + len + 4, SEEK_SET) < 0)
				return FALSE;
			data->file_offset = data->file_offset + len + 4;
			break;
		}

		if(data->file_offset != data->next_offset)
			return FALSE;
	}

	return FALSE;
}

static x_bool png_load(struct bitmap ** bitmap, const char * filename)
{
	struct png_data * data;
	struct stat st;
	x_s32 fd;

	if(stat(filename, &st) != 0)
		return FALSE;

	if(S_ISDIR(st.st_mode))
		return FALSE;

	fd = open(filename, O_RDONLY, (S_IRUSR|S_IRGRP|S_IROTH));
	if(fd < 0)
		return FALSE;

	data = malloc(sizeof(struct png_data));
	if(!data)
	{
		close(fd);
		return FALSE;
	}

    data->file = fd;
    data->file_offset = 0;
    data->bitmap = bitmap;

    if(!png_decode(data))
    {
    	free(data->image_data);
    	free(data);
    	bitmap_destroy(*bitmap);
        *bitmap = 0;
    	close(fd);
    	return FALSE;
    }

	free(data->image_data);
	free(data);
	close(fd);

	return TRUE;
}

/*
 * png bitmap reader
 */
static struct bitmap_reader bitmap_reader_png = {
	.extension		= ".png",
	.load			= png_load,
};

static __init void bitmap_reader_png_init(void)
{
	if(!register_bitmap_reader(&bitmap_reader_png))
		LOG_E("register 'png' bitmap reader fail");
}

static __exit void bitmap_reader_png_exit(void)
{
	if(!unregister_bitmap_reader(&bitmap_reader_png))
		LOG_E("unregister 'png' bitmap reader fail");
}

module_init(bitmap_reader_png_init, LEVEL_POSTCORE);
module_exit(bitmap_reader_png_exit, LEVEL_POSTCORE);
