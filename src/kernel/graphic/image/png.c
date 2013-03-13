/*
 * kernel/graphic/image/png.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
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

#include <graphic/image.h>

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
	s32_t * values, * maxval, * offset;
	s32_t num_values, max_length;
};

struct png_data
{
	struct stream_t * stream;
	struct surface_t * surface;
	u32_t file_offset;

	s32_t bit_count, bit_save;

	u32_t next_offset;

	s32_t image_width, image_height, bpp, is_16bit, raw_bytes;
	u8_t * image_data;

	s32_t inside_idat, idat_remain;

	s32_t code_values[DEFLATE_HLIT_MAX];
	s32_t code_maxval[DEFLATE_HUFF_LEN];
	s32_t code_offset[DEFLATE_HUFF_LEN];

	s32_t dist_values[DEFLATE_HDIST_MAX];
	s32_t dist_maxval[DEFLATE_HUFF_LEN];
	s32_t dist_offset[DEFLATE_HUFF_LEN];

	struct huff_table code_table;
	struct huff_table dist_table;

	u8_t slide[WINDOW_SIZE];
	s32_t wp;

	u8_t * cur_rgb;

	s32_t cur_column, cur_filter, first_line;
};

/* png magic number */
static const u8_t png_magic[8] = {
	0x89, 0x50, 0x4e, 0x47, 0xd, 0xa, 0x1a, 0x0a
};

/* order of the bit length code lengths */
static const u8_t bitorder[] = {
	16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
};

/* copy lengths for literal codes 257..285 */
static const s32_t cplens[] = {
	3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
	35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0, 0
};

/* extra bits for literal codes 257..285 */
static const u8_t cplext[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
	3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 99, 99
};

/* copy offsets for distance codes 0..29 */
static const s32_t cpdist[] = {
	1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
	257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145,
	8193, 12289, 16385, 24577
};

/* extra bits for distance codes */
static const u8_t cpdext[] = {
	0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
	7, 7, 8, 8, 9, 9, 10, 10, 11, 11,
	12, 12, 13, 13
};

static u32_t png_get_dword(struct png_data * data)
{
	u32_t r = 0;

	data->file_offset += stream_read(data->stream, &r, 1, sizeof(u32_t));
	return be32_to_cpu(r);
}

static u8_t png_get_byte(struct png_data * data)
{
	u8_t r;
	u32_t len, type;

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
	data->file_offset += stream_read(data->stream, &r, 1, sizeof(u8_t));

	if(data->inside_idat)
		data->idat_remain--;

	return r;
}

static s32_t png_get_bits(struct png_data * data, s32_t num)
{
	s32_t code, shift;
	s32_t n;

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

		code += (s32_t)(data->bit_save & ((1 << n) - 1)) << shift;
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

static void png_init_huff_table(struct huff_table * ht, s32_t cur_maxlen, s32_t * cur_values, s32_t * cur_maxval, s32_t * cur_offset)
{
	ht->values = cur_values;
	ht->maxval = cur_maxval;
	ht->offset = cur_offset;
	ht->num_values = 0;
	ht->max_length = cur_maxlen;
	memset(cur_maxval, 0, sizeof(s32_t) * cur_maxlen);
}

static void png_insert_huff_item(struct huff_table * ht, s32_t code, s32_t len)
{
	s32_t i, n;

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
	s32_t base, ofs, i;

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

static s32_t png_get_huff_code(struct png_data * data, struct huff_table * ht)
{
	s32_t code, i;

	code = 0;
	for(i = 0; i < ht->max_length; i++)
	{
		code = (code << 1) + png_get_bits(data, 1);
		if(code < ht->maxval[i])
			return ht->values[code + ht->offset[i]];
	}

	return 0;
}

static bool_t png_init_fixed_block(struct png_data * data)
{
	s32_t i;

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

static bool_t png_init_dynamic_block(struct png_data * data)
{
	s32_t nl, nd, nb, i, prev;
	struct huff_table cl;
	s32_t cl_values[sizeof(bitorder)];
	s32_t cl_maxval[8];
	s32_t cl_offset[8];
	u8_t lens[DEFLATE_HCLEN_MAX];
	s32_t n, code, c;
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

static bool_t png_output_byte(struct png_data * data, u8_t n)
{
	s32_t row_bytes;
	s32_t i;
	s32_t a, b, c, pa, pb, pc;
	u8_t * blank_line;
	u8_t * cur;
	u8_t * left;
	u8_t * up;
	u8_t * upper_left;

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

static bool_t png_read_dynamic_block(struct png_data * data)
{
	s32_t n;
	s32_t len, dist, pos;

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

static bool_t png_decode_image_header(struct png_data * data)
{
	s32_t color_type;
	s32_t color_bits;

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
		data->surface = surface_alloc(NULL, data->image_width, data->image_height, PIXEL_FORMAT_BGR_888);
		if(!data->surface)
			return FALSE;
		data->bpp = 3;
	}
	else if(color_type == PNG_COLOR_TYPE_RGBA)
	{
		data->surface = surface_alloc(NULL, data->image_width, data->image_height, PIXEL_FORMAT_ABGR_8888);
		if(!data->surface)
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
		data->cur_rgb = data->surface->pixels;
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

static bool_t png_decode_image_data(struct png_data * data)
{
	u8_t cmf, flg;
	s32_t final;
	s32_t block_type;
	u16_t i, len;

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
			len += ((u16_t)png_get_byte(data)) << 8;

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
	s32_t i;
	u8_t *d1, *d2;

	d1 = data->surface->pixels;
	d2 = data->image_data + 1;

	/* only copy the upper 8 bit */
	for(i = 0; i < (data->image_width * data->image_height * data->bpp >> 1); i++, d1++, d2+=2)
		*d1 = *d2;
}

static bool_t png_decode(struct png_data * data)
{
	u8_t magic[8];
	u32_t len, type;

	if(stream_read(data->stream, &magic[0], 1, 8) != 8)
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
			if(!stream_seek(data->stream, data->file_offset + len + 4, STREAM_SEEK_SET))
				return FALSE;
			data->file_offset = data->file_offset + len + 4;
			break;
		}

		if(data->file_offset != data->next_offset)
			return FALSE;
	}

	return FALSE;
}

static struct surface_t * png_load(const char * filename)
{
	struct stream_t * stream;
	struct surface_t * surface;
	struct png_data * data;

	stream = stream_alloc(filename, "r");
	if(!stream)
		return NULL;

	data = malloc(sizeof(struct png_data));
	if(!data)
	{
		stream_free(stream);
		return FALSE;
	}

	memset(data, 0, sizeof(struct png_data));
    data->stream = stream;
    data->file_offset = 0;

    if(!png_decode(data))
    {
    	surface_free(data->surface);
    	stream_free(data->stream);
    	free(data->image_data);
    	free(data);
    	return FALSE;
    }

    surface = data->surface;

	stream_free(data->stream);
	free(data->image_data);
	free(data);

	return surface;
}

static struct image_loader image_loader_png = {
	.extension		= ".png",
	.load			= png_load,
};

static __init void image_loader_png_init(void)
{
	if(!register_image_loader(&image_loader_png))
		LOG_E("register 'png' image loader fail");
}

static __exit void image_loader_png_exit(void)
{
	if(!unregister_image_loader(&image_loader_png))
		LOG_E("unregister 'png' image loader fail");
}

core_initcall(image_loader_png_init);
core_exitcall(image_loader_png_exit);
