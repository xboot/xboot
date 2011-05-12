/*
 * drivers/fb/bitmap/jpeg.c
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

#define SHIFT_BITS					(8)
#define JPEG_UNIT_SIZE				(8)
#define CONST(x)					((s32_t)((x) * (1L << SHIFT_BITS) + 0.5))

enum
{
	JPEG_MARKER_SOI					= 0xd8,
	JPEG_MARKER_EOI					= 0xd9,
	JPEG_MARKER_DHT					= 0xc4,
	JPEG_MARKER_DQT					= 0xdb,
	JPEG_MARKER_SOF					= 0xc0,
	JPEG_MARKER_SOS					= 0xda,
};

typedef s32_t jpeg_data_unit[64];

struct jpeg_data
{
	s32_t file;
	u32_t file_offset;
	struct bitmap ** bitmap;

	s32_t image_width;
	s32_t image_height;

	u8_t *huff_value[4];
	s32_t huff_offset[4][16];
	s32_t huff_maxval[4][16];

	u8_t quan_table[2][64];
	s32_t comp_index[3][3];

	jpeg_data_unit ydu[4];
	jpeg_data_unit crdu;
	jpeg_data_unit cbdu;

	s32_t vs, hs;

	s32_t dc_value[3];

	s32_t bit_mask, bit_save;
};

static const u8_t jpeg_zigzag_order[64] = {
	0, 1, 8, 16, 9, 2, 3, 10,
	17, 24, 32, 25, 18, 11, 4, 5,
	12, 19, 26, 33, 40, 48, 41, 34,
	27, 20, 13, 6, 7, 14, 21, 28,
	35, 42, 49, 56, 57, 50, 43, 36,
	29, 22, 15, 23, 30, 37, 44, 51,
	58, 59, 52, 45, 38, 31, 39, 46,
	53, 60, 61, 54, 47, 55, 62, 63
};

static u8_t jpeg_get_byte(struct jpeg_data * data)
{
	u8_t r = 0;

	data->file_offset += read(data->file, &r, sizeof(u8_t));
	return r;
}

static u16_t jpeg_get_word(struct jpeg_data * data)
{
	u16_t r = 0;

	data->file_offset += read(data->file, &r, sizeof(u16_t));
	return be16_to_cpu(r);
}

static s32_t jpeg_get_bit(struct jpeg_data * data)
{
	s32_t ret;

	if(data->bit_mask == 0)
	{
		data->bit_save = jpeg_get_byte(data);
		if(data->bit_save == 0xff)
		{
			if(jpeg_get_byte(data) != 0)
				return 0;
		}
		data->bit_mask = 0x80;
	}

	ret = ((data->bit_save & data->bit_mask) != 0);
	data->bit_mask >>= 1;
	return ret;
}

static s32_t jpeg_get_number(struct jpeg_data * data, s32_t num)
{
	s32_t value, i, msb;

	if(num == 0)
		return 0;

	msb = value = jpeg_get_bit(data);
	for(i = 1; i < num; i++)
		value = (value << 1) + (jpeg_get_bit(data) != 0);
	if(!msb)
		value += 1 - (1 << num);

	return value;
}

static s32_t jpeg_get_huff_code(struct jpeg_data * data, s32_t id)
{
	s32_t code, i;

	code = 0;
	for(i = 0; i < sizeof(data->huff_maxval[id]); i++)
	{
		code <<= 1;
		if(jpeg_get_bit (data))
			code++;
		if(code < data->huff_maxval[id][i])
			return data->huff_value[id][code + data->huff_offset[id][i]];
	}

	return 0;
}

static bool_t jpeg_decode_huff_table(struct jpeg_data * data)
{
	s32_t id, ac, i, n, base, ofs;
	u32_t next_marker;
	u8_t count[16];

	next_marker = data->file_offset;
	next_marker += jpeg_get_word(data);

	while(data->file_offset + sizeof(count) + 1 <= next_marker)
	{
		id = jpeg_get_byte(data);
		ac = (id >> 4) & 0x1;
		id &= 0xF;
		if(id > 1)
			return FALSE;

		if(read(data->file, &count, sizeof(count)) != sizeof(count))
			return FALSE;
		data->file_offset += sizeof(count);

		n = 0;
		for(i = 0; i < sizeof(count); i++)
			n += count[i];

		id += ac * 2;
		data->huff_value[id] = malloc(n);
		if(!data->huff_value[id])
			return FALSE;

		if(read(data->file, data->huff_value[id], n) != n)
			return FALSE;
		data->file_offset += n;

		base = 0;
		ofs = 0;
		for(i = 0; i < sizeof(count); i++)
		{
			base += count[i];
			ofs += count[i];

			data->huff_maxval[id][i] = base;
			data->huff_offset[id][i] = ofs - base;

			base <<= 1;
		}
	}

	if(data->file_offset != next_marker)
		return FALSE;

	return TRUE;
}

static bool_t jpeg_decode_quan_table(struct jpeg_data * data)
{
	s32_t id;
	u32_t next_marker;

	next_marker = data->file_offset;
	next_marker += jpeg_get_word(data);

	while(data->file_offset + sizeof(data->quan_table[id]) + 1 <= next_marker)
	{
		id = jpeg_get_byte(data);
		if(id >= 0x10)
			return FALSE;

		if(id > 1)
			return FALSE;

		if(read(data->file, &data->quan_table[id], sizeof(data->quan_table[id])) != sizeof(data->quan_table[id]))
			return FALSE;
		data->file_offset += 64;
	}

	if(data->file_offset != next_marker)
		return FALSE;

	return TRUE;
}

static bool_t jpeg_decode_sof(struct jpeg_data * data)
{
	s32_t i, cc;
	u32_t next_marker;
	s32_t id, ss;

	next_marker = data->file_offset;
	next_marker += jpeg_get_word(data);

	if(jpeg_get_byte(data) != 8)
		return FALSE;

	data->image_height = jpeg_get_word(data);
	data->image_width = jpeg_get_word(data);

	if((!data->image_height) || (!data->image_width))
		return FALSE;

	cc = jpeg_get_byte(data);
	if(cc != 3)
		return FALSE;

	for (i = 0; i < cc; i++)
	{
		id = jpeg_get_byte(data) - 1;
		if((id < 0) || (id >= 3))
			return FALSE;

		ss = jpeg_get_byte(data);
		if(!id)
		{
			data->vs = ss & 0xF;
			data->hs = ss >> 4;
			if((data->vs > 2) || (data->hs > 2))
				return FALSE;
		}
		else if(ss != 0x11)
			return FALSE;

		data->comp_index[id][0] = jpeg_get_byte(data);
	}

	if(data->file_offset != next_marker)
		return FALSE;

	return TRUE;
}

static void jpeg_idct_transform(jpeg_data_unit du)
{
	s32_t *pd;
	s32_t i;
	s32_t t0, t1, t2, t3, t4, t5, t6, t7;
	s32_t v0, v1, v2, v3, v4;

	pd = du;
	for(i = 0; i < JPEG_UNIT_SIZE; i++, pd++)
	{
		if((pd[JPEG_UNIT_SIZE * 1] | pd[JPEG_UNIT_SIZE * 2] |
				pd[JPEG_UNIT_SIZE * 3] | pd[JPEG_UNIT_SIZE * 4] |
				pd[JPEG_UNIT_SIZE * 5] | pd[JPEG_UNIT_SIZE * 6] |
				pd[JPEG_UNIT_SIZE * 7]) == 0)
		{
			pd[JPEG_UNIT_SIZE * 0] <<= SHIFT_BITS;

			pd[JPEG_UNIT_SIZE * 1] = pd[JPEG_UNIT_SIZE * 2]
			= pd[JPEG_UNIT_SIZE * 3] = pd[JPEG_UNIT_SIZE * 4]
			= pd[JPEG_UNIT_SIZE * 5] = pd[JPEG_UNIT_SIZE * 6]
			= pd[JPEG_UNIT_SIZE * 7] = pd[JPEG_UNIT_SIZE * 0];

			continue;
		}

		t0 = pd[JPEG_UNIT_SIZE * 0];
		t1 = pd[JPEG_UNIT_SIZE * 2];
		t2 = pd[JPEG_UNIT_SIZE * 4];
		t3 = pd[JPEG_UNIT_SIZE * 6];

		v4 = (t1 + t3) * CONST(0.541196100);

		v0 = ((t0 + t2) << SHIFT_BITS);
		v1 = ((t0 - t2) << SHIFT_BITS);
		v2 = v4 - t3 * CONST(1.847759065);
		v3 = v4 + t1 * CONST(0.765366865);

		t0 = v0 + v3;
		t3 = v0 - v3;
		t1 = v1 + v2;
		t2 = v1 - v2;

		t4 = pd[JPEG_UNIT_SIZE * 7];
		t5 = pd[JPEG_UNIT_SIZE * 5];
		t6 = pd[JPEG_UNIT_SIZE * 3];
		t7 = pd[JPEG_UNIT_SIZE * 1];

		v0 = t4 + t7;
		v1 = t5 + t6;
		v2 = t4 + t6;
		v3 = t5 + t7;

		v4 = (v2 + v3) * CONST(1.175875602);

		v0 *= CONST(0.899976223);
		v1 *= CONST(2.562915447);
		v2 = v2 * CONST(1.961570560) - v4;
		v3 = v3 * CONST(0.390180644) - v4;

		t4 = t4 * CONST(0.298631336) - v0 - v2;
		t5 = t5 * CONST(2.053119869) - v1 - v3;
		t6 = t6 * CONST(3.072711026) - v1 - v2;
		t7 = t7 * CONST(1.501321110) - v0 - v3;

		pd[JPEG_UNIT_SIZE * 0] = t0 + t7;
		pd[JPEG_UNIT_SIZE * 7] = t0 - t7;
		pd[JPEG_UNIT_SIZE * 1] = t1 + t6;
		pd[JPEG_UNIT_SIZE * 6] = t1 - t6;
		pd[JPEG_UNIT_SIZE * 2] = t2 + t5;
		pd[JPEG_UNIT_SIZE * 5] = t2 - t5;
		pd[JPEG_UNIT_SIZE * 3] = t3 + t4;
		pd[JPEG_UNIT_SIZE * 4] = t3 - t4;
	}

	pd = du;
	for(i = 0; i < JPEG_UNIT_SIZE; i++, pd += JPEG_UNIT_SIZE)
	{
		if((pd[1] | pd[2] | pd[3] | pd[4] | pd[5] | pd[6] | pd[7]) == 0)
		{
			pd[0] >>= (SHIFT_BITS + 3);
			pd[1] = pd[2] = pd[3] = pd[4] = pd[5] = pd[6] = pd[7] = pd[0];
			continue;
		}

		v4 = (pd[2] + pd[6]) * CONST(0.541196100);

		v0 = (pd[0] + pd[4]) << SHIFT_BITS;
		v1 = (pd[0] - pd[4]) << SHIFT_BITS;
		v2 = v4 - pd[6] * CONST(1.847759065);
		v3 = v4 + pd[2] * CONST(0.765366865);

		t0 = v0 + v3;
		t3 = v0 - v3;
		t1 = v1 + v2;
		t2 = v1 - v2;

		t4 = pd[7];
		t5 = pd[5];
		t6 = pd[3];
		t7 = pd[1];

		v0 = t4 + t7;
		v1 = t5 + t6;
		v2 = t4 + t6;
		v3 = t5 + t7;

		v4 = (v2 + v3) * CONST(1.175875602);

		v0 *= CONST(0.899976223);
		v1 *= CONST(2.562915447);
		v2 = v2 * CONST(1.961570560) - v4;
		v3 = v3 * CONST(0.390180644) - v4;

		t4 = t4 * CONST(0.298631336) - v0 - v2;
		t5 = t5 * CONST(2.053119869) - v1 - v3;
		t6 = t6 * CONST(3.072711026) - v1 - v2;
		t7 = t7 * CONST(1.501321110) - v0 - v3;

		pd[0] = (t0 + t7) >> (SHIFT_BITS * 2 + 3);
		pd[7] = (t0 - t7) >> (SHIFT_BITS * 2 + 3);
		pd[1] = (t1 + t6) >> (SHIFT_BITS * 2 + 3);
		pd[6] = (t1 - t6) >> (SHIFT_BITS * 2 + 3);
		pd[2] = (t2 + t5) >> (SHIFT_BITS * 2 + 3);
		pd[5] = (t2 - t5) >> (SHIFT_BITS * 2 + 3);
		pd[3] = (t3 + t4) >> (SHIFT_BITS * 2 + 3);
		pd[4] = (t3 - t4) >> (SHIFT_BITS * 2 + 3);
	}

	for(i = 0; i < JPEG_UNIT_SIZE * JPEG_UNIT_SIZE; i++)
	{
		du[i] += 128;

		if(du[i] < 0)
			du[i] = 0;
		if(du[i] > 255)
			du[i] = 255;
	}
}

static void jpeg_decode_du(struct jpeg_data * data, s32_t id, jpeg_data_unit du)
{
	s32_t pos, h1, h2, qt;
	s32_t num, val;

	memset(du, 0, sizeof(jpeg_data_unit));

	qt = data->comp_index[id][0];
	h1 = data->comp_index[id][1];
	h2 = data->comp_index[id][2];

	data->dc_value[id] += jpeg_get_number(data, jpeg_get_huff_code(data, h1));

	du[0] = data->dc_value[id] * (s32_t) data->quan_table[qt][0];
	pos = 1;
	while(pos < sizeof(data->quan_table[qt]))
	{
		num = jpeg_get_huff_code (data, h2);
		if(!num)
			break;

		val = jpeg_get_number(data, num & 0xf);
		num >>= 4;
		pos += num;
		du[jpeg_zigzag_order[pos]] = val * (s32_t) data->quan_table[qt][pos];
		pos++;
	}

	jpeg_idct_transform(du);
}

static void jpeg_ycrcb_to_rgb(s32_t yy, s32_t cr, s32_t cb, u8_t * rgb)
{
	s32_t dd;

	cr -= 128;
	cb -= 128;

	/* red */
	dd = yy + ((cr * CONST(1.402)) >> SHIFT_BITS);
	if(dd < 0)
		dd = 0;
	if(dd > 255)
		dd = 255;
	*(rgb++) = dd;

	/* green */
	dd = yy - ((cb * CONST(0.34414) + cr * CONST(0.71414)) >> SHIFT_BITS);
	if(dd < 0)
		dd = 0;
	if(dd > 255)
		dd = 255;
	*(rgb++) = dd;

	/* blue */
	dd = yy + ((cb * CONST(1.772)) >> SHIFT_BITS);
	if(dd < 0)
		dd = 0;
	if(dd > 255)
		dd = 255;
	*(rgb++) = dd;
}

static bool_t jpeg_decode_sos(struct jpeg_data * data)
{
	s32_t i, cc, r1, c1, nr1, nc1, vb, hb;
	u8_t *ptr1;
	u32_t data_offset;
	s32_t id, ht;
	s32_t r2, c2, nr2, nc2;
	u8_t *ptr2;
	s32_t i0, yy, cr, cb;

	data_offset = data->file_offset;
	data_offset += jpeg_get_word(data);

	cc = jpeg_get_byte(data);

	if(cc != 3)
		return FALSE;

	for(i = 0; i < cc; i++)
	{
		id = jpeg_get_byte(data) - 1;
		if((id < 0) || (id >= 3))
			return FALSE;

		ht = jpeg_get_byte(data);
		data->comp_index[id][1] = (ht >> 4);
		data->comp_index[id][2] = (ht & 0xf) + 2;
	}

	jpeg_get_byte (data);
	jpeg_get_word (data);

	if(data->file_offset != data_offset)
		return FALSE;

	if(!bitmap_create (data->bitmap, data->image_width, data->image_height, BITMAP_FORMAT_RGB_888))
		return FALSE;

	data->bit_mask = 0x0;

	vb = data->vs * 8;
	hb = data->hs * 8;
	nr1 = (data->image_height + vb - 1) / vb;
	nc1 = (data->image_width + hb - 1) / hb;

	ptr1 = (*data->bitmap)->data;
	for(r1 = 0; r1 < nr1; r1++, ptr1 += (vb * data->image_width - hb * nc1) * 3)
	{
		for(c1 = 0; c1 < nc1; c1++, ptr1 += hb * 3)
		{
			for(r2 = 0; r2 < data->vs; r2++)
				for(c2 = 0; c2 < data->hs; c2++)
					jpeg_decode_du(data, 0, data->ydu[r2 * 2 + c2]);

			jpeg_decode_du(data, 1, data->cbdu);
			jpeg_decode_du(data, 2, data->crdu);

			nr2 = (r1 == nr1 - 1) ? (data->image_height - r1 * vb) : vb;
			nc2 = (c1 == nc1 - 1) ? (data->image_width - c1 * hb) : hb;

			ptr2 = ptr1;
			for(r2 = 0; r2 < nr2; r2++, ptr2 += (data->image_width - nc2) * 3)
			{
				for(c2 = 0; c2 < nc2; c2++, ptr2 += 3)
				{
					i0 = (r2 / data->vs) * 8 + (c2 / data->hs);
					cr = data->crdu[i0];
					cb = data->cbdu[i0];
					yy = data->ydu[(r2 / 8) * 2 + (c2 / 8)][(r2 % 8) * 8 + (c2 % 8)];

					jpeg_ycrcb_to_rgb(yy, cr, cb, ptr2);
				}
			}
		}
	}

	return TRUE;
}

static u8_t jpeg_get_marker(struct jpeg_data * data)
{
	u8_t r;

	r = jpeg_get_byte(data);

	if(r != 0xff)
		return 0;

	return jpeg_get_byte(data);
}

static bool_t jpeg_decode(struct jpeg_data * data)
{
	u8_t marker;
	u16_t sz;

	if(jpeg_get_marker(data) != JPEG_MARKER_SOI)
		return FALSE;

	while(1)
	{
		marker = jpeg_get_marker(data);

		switch(marker)
		{
		case JPEG_MARKER_DHT:
			jpeg_decode_huff_table(data);
			break;

		case JPEG_MARKER_DQT:
			jpeg_decode_quan_table(data);
			break;

		case JPEG_MARKER_SOF:
			jpeg_decode_sof(data);
			break;

		case JPEG_MARKER_SOS:
			jpeg_decode_sos(data);
			break;

		case JPEG_MARKER_EOI:
			return TRUE;

		default:
			sz = jpeg_get_word(data);

			if(lseek(data->file, data->file_offset + sz - 2, SEEK_SET) < 0)
				return FALSE;
			data->file_offset = data->file_offset + sz - 2;
			break;
		}
	}

	return FALSE;
}

static bool_t jpeg_load(struct bitmap ** bitmap, const char * filename)
{
	struct jpeg_data * data;
	struct stat st;
	s32_t fd;
	s32_t i;

	if(stat(filename, &st) != 0)
		return FALSE;

	if(S_ISDIR(st.st_mode))
		return FALSE;

	fd = open(filename, O_RDONLY, (S_IRUSR|S_IRGRP|S_IROTH));
	if(fd < 0)
		return FALSE;

	data = malloc(sizeof(struct jpeg_data));
	if(!data)
	{
		close(fd);
		return FALSE;
	}

    data->file = fd;
    data->file_offset = 0;
    data->bitmap = bitmap;

    if(!jpeg_decode(data))
    {
        for(i = 0; i < 4; i++)
        {
        	if(data->huff_value[i])
        		free(data->huff_value[i]);
        }
    	free(data);

    	bitmap_destroy(*bitmap);
        *bitmap = 0;
    	close(fd);
    	return FALSE;
    }

    for(i = 0; i < 4; i++)
    {
    	if(data->huff_value[i])
    		free(data->huff_value[i]);
    }
	free(data);
	close(fd);

	return TRUE;
}

/*
 * jpeg bitmap reader
 */
static struct bitmap_reader bitmap_reader_jpeg = {
	.extension		= ".jpeg",
	.load			= jpeg_load,
};

/*
 * jpg bitmap reader
 */
static struct bitmap_reader bitmap_reader_jpg = {
	.extension		= ".jpg",
	.load			= jpeg_load,
};

static __init void bitmap_reader_jpeg_init(void)
{
	if(!register_bitmap_reader(&bitmap_reader_jpeg))
		LOG_E("register 'jpeg' bitmap reader fail");
	if(!register_bitmap_reader(&bitmap_reader_jpg))
		LOG_E("register 'jpg' bitmap reader fail");
}

static __exit void bitmap_reader_jpeg_exit(void)
{
	if(!unregister_bitmap_reader(&bitmap_reader_jpeg))
		LOG_E("unregister 'jpeg' bitmap reader fail");
	if(!unregister_bitmap_reader(&bitmap_reader_jpg))
		LOG_E("unregister 'jpg' bitmap reader fail");
}

module_init(bitmap_reader_jpeg_init, LEVEL_POSTCORE);
module_exit(bitmap_reader_jpeg_exit, LEVEL_POSTCORE);
