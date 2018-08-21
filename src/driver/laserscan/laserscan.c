/*
 * driver/laserscan/laserscan.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xboot.h>
#include <laserscan/laserscan.h>

static ssize_t laserscan_read_color(struct kobj_t * kobj, void * buf, size_t size)
{
	struct laserscan_t * l = (struct laserscan_t *)kobj->priv;
	u8_t r, g, b, a;
	laserscan_get_color(l, &r, &g, &b, &a);
	return sprintf(buf, "0x%08x", (a << 24) | (r << 16) | (g << 8) | (b << 0));
}

static ssize_t laserscan_write_color(struct kobj_t * kobj, void * buf, size_t size)
{
	struct laserscan_t * l = (struct laserscan_t *)kobj->priv;
	u32_t c = strtoul(buf, NULL, 0);
	u8_t r = (c >> 16) & 0xff;
	u8_t g = (c >> 8) & 0xff;
	u8_t b = (c >> 0) & 0xff;
	u8_t a = (c >> 24) & 0xff;
	laserscan_set_color(l, r, g, b, a);
	return size;
}

struct laserscan_t * search_laserscan(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_LASERSCAN);
	if(!dev)
		return NULL;
	return (struct laserscan_t *)dev->priv;
}

struct laserscan_t * search_first_laserscan(void)
{
	struct device_t * dev;

	dev = search_first_device(DEVICE_TYPE_LASERSCAN);
	if(!dev)
		return NULL;
	return (struct laserscan_t *)dev->priv;
}

bool_t register_laserscan(struct device_t ** device,struct laserscan_t * l)
{
	struct device_t * dev;

	if(!l || !l->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(l->name);
	dev->type = DEVICE_TYPE_LASERSCAN;
	dev->priv = l;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "color", laserscan_read_color, laserscan_write_color, l);

	if(!register_device(dev))
	{
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return FALSE;
	}

	if(device)
		*device = dev;
	return TRUE;
}

bool_t unregister_laserscan(struct laserscan_t * l)
{
	struct device_t * dev;

	if(!l || !l->name)
		return FALSE;

	dev = search_device(l->name, DEVICE_TYPE_LASERSCAN);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

void laserscan_perspective(struct laserscan_t * l, float x, float y)
{
	if(l && l->perspective)
		l->perspective(l, x, y);
}

void laserscan_translate(struct laserscan_t * l, float x, float y)
{
	if(l && l->translate)
		l->translate(l, x, y);
}

void laserscan_scale(struct laserscan_t * l, float x, float y)
{
	if(l && l->scale)
		l->scale(l, x, y);
}

void laserscan_shear(struct laserscan_t * l, float x, float y)
{
	if(l && l->shear)
		l->shear(l, x, y);
}

void laserscan_set_color(struct laserscan_t * l, u8_t r, u8_t g, u8_t b, u8_t a)
{
	if(l && l->set_color)
		l->set_color(l, r, g, b, a);
}

void laserscan_get_color(struct laserscan_t * l, u8_t * r, u8_t * g, u8_t * b, u8_t * a)
{
	if(l && l->get_color)
		l->get_color(l, r, g, b, a);
}

void laserscan_move_to(struct laserscan_t * l, float x, float y)
{
	if(l && l->move_to)
		l->move_to(l, x, y);
}

void laserscan_rel_move_to(struct laserscan_t * l, float dx, float dy)
{
	if(l && l->rel_move_to)
		l->rel_move_to(l, dx, dy);
}

void laserscan_line_to(struct laserscan_t * l, float x, float y)
{
	if(l && l->line_to)
		l->line_to(l, x, y);
}

void laserscan_rel_line_to(struct laserscan_t * l, float dx, float dy)
{
	if(l && l->rel_line_to)
		l->rel_line_to(l, dx, dy);
}

void laserscan_curve_to(struct laserscan_t * l, float x1, float y1, float x2, float y2, float x3, float y3)
{
	if(l && l->curve_to)
		l->curve_to(l, x1, y1, x2, y2, x3, y3);
}

void laserscan_rel_curve_to(struct laserscan_t * l, float dx1, float dy1, float dx2, float dy2, float dx3, float dy3)
{
	if(l && l->rel_curve_to)
		l->rel_curve_to(l, dx1, dy1, dx2, dy2, dx3, dy3);
}

void laserscan_arc(struct laserscan_t * l, float xc, float yc, float r, float a1, float a2)
{
	if(l && l->arc)
		l->arc(l, xc, yc, r, a1, a2);
}

void laserscan_arc_negative(struct laserscan_t * l, float xc, float yc, float r, float a1, float a2)
{
	if(l && l->arc_negative)
		l->arc_negative(l, xc, yc, r, a1, a2);
}

void laserscan_rectangle(struct laserscan_t * l, float x, float y, float w, float h)
{
	float tx = x + w;
	float ty = y + h;

	laserscan_move_to(l, x, y);
	laserscan_line_to(l, tx, y);
	laserscan_line_to(l, tx, ty);
	laserscan_line_to(l, x, ty);
	laserscan_line_to(l, x, y);
}

void laserscan_seekbar(struct laserscan_t * l, float x, float y, float w, float h, float v)
{
	float tx = x + w;
	float ty = y + h;
	float tx1, tx2, ty1, ty2;

	if(v < 0)
		v = 0;
	else if(v > 1)
		v = 1;

	if(h < w)
	{
		tx1 = x + (w - h) * v;
		tx2 = x + (w - h) * v + h;
		ty1 = y - h;
		ty2 = y + h + h;

		laserscan_move_to(l, x, y);
		laserscan_line_to(l, tx1, y);
		laserscan_line_to(l, tx1, ty1);
		laserscan_line_to(l, tx2, ty1);
		laserscan_line_to(l, tx2, y);
		laserscan_line_to(l, tx, y);
		laserscan_line_to(l, tx, ty);
		laserscan_line_to(l, tx2, ty);
		laserscan_line_to(l, tx2, ty2);
		laserscan_line_to(l, tx1, ty2);
		laserscan_line_to(l, tx1, ty);
		laserscan_line_to(l, x, ty);
		laserscan_line_to(l, x, y);
	}
	else
	{
		tx1 = x - w;
		tx2 = x + w + w;
		ty1 = y + (h - w) * v;
		ty2 = y + (h - w) * v + w;

		laserscan_move_to(l, x, y);
		laserscan_line_to(l, tx, y);
		laserscan_line_to(l, tx, ty1);
		laserscan_line_to(l, tx2, ty1);
		laserscan_line_to(l, tx2, ty2);
		laserscan_line_to(l, tx, ty2);
		laserscan_line_to(l, tx, ty);
		laserscan_line_to(l, x, ty);
		laserscan_line_to(l, x, ty2);
		laserscan_line_to(l, tx1, ty2);
		laserscan_line_to(l, tx1, ty1);
		laserscan_line_to(l, x, ty1);
		laserscan_line_to(l, x, y);
	}
}

enum ilda_state_t {
	ILDA_STATE_MAGIC				= 0,
	ILDA_STATE_RESERVED1			= 1,
	ILDA_STATE_FORMAT				= 2,
	ILDA_STATE_NAME					= 3,
	ILDA_STATE_COMPANY				= 4,
	ILDA_STATE_COUNT				= 5,
	ILDA_STATE_FRAME_NUM			= 6,
	ILDA_STATE_FRAME_CNT			= 7,
	ILDA_STATE_SCANNER				= 8,
	ILDA_STATE_RESERVED2			= 9,
	ILDA_STATE_RECORD				= 10,
};

enum ilda_coord_format_t {
	ILDA_COORD_FORMAT_3D_INDEX		= 0,
	ILDA_COORD_FORMAT_2D_INDEX		= 1,
	ILDA_COORD_FORMAT_PALETTE_COLOR	= 2,
	ILDA_COORD_FORMAT_3D_TRUE_COLOR	= 4,
	ILDA_COORD_FORMAT_2D_TRUE_COLOR	= 5,
};

struct ilda_ctx_t {
	enum ilda_state_t state;
	uint8_t buf[32];
	int index;
	enum ilda_coord_format_t format;
	int count;
	int fnum;
	int fcnt;
	int record;
};

struct ilda_point_t {
	float x, y, z;
	uint8_t r, g, b;
	uint8_t a;
	int index;
};

static const uint8_t ilda_default_palette[256][3] = {
	{ 0, 0, 0 },
	{ 255, 255, 255 },
	{ 255, 0, 0 },
	{ 255, 255, 0 },
	{ 0, 255, 0 },
	{ 0, 255, 255 },
	{ 0, 0, 255 },
	{ 255, 0, 255 },
	{ 255, 128, 128 },
	{ 255, 140, 128 },
	{ 255, 151, 128 },
	{ 255, 163, 128 },
	{ 255, 174, 128 },
	{ 255, 186, 128 },
	{ 255, 197, 128 },
	{ 255, 209, 128 },
	{ 255, 220, 128 },
	{ 255, 232, 128 },
	{ 255, 243, 128 },
	{ 255, 255, 128 },
	{ 243, 255, 128 },
	{ 232, 255, 128 },
	{ 220, 255, 128 },
	{ 209, 255, 128 },
	{ 197, 255, 128 },
	{ 186, 255, 128 },
	{ 174, 255, 128 },
	{ 163, 255, 128 },
	{ 151, 255, 128 },
	{ 140, 255, 128 },
	{ 128, 255, 128 },
	{ 128, 255, 140 },
	{ 128, 255, 151 },
	{ 128, 255, 163 },
	{ 128, 255, 174 },
	{ 128, 255, 186 },
	{ 128, 255, 197 },
	{ 128, 255, 209 },
	{ 128, 255, 220 },
	{ 128, 255, 232 },
	{ 128, 255, 243 },
	{ 128, 255, 255 },
	{ 128, 243, 255 },
	{ 128, 232, 255 },
	{ 128, 220, 255 },
	{ 128, 209, 255 },
	{ 128, 197, 255 },
	{ 128, 186, 255 },
	{ 128, 174, 255 },
	{ 128, 163, 255 },
	{ 128, 151, 255 },
	{ 128, 140, 255 },
	{ 128, 128, 255 },
	{ 140, 128, 255 },
	{ 151, 128, 255 },
	{ 163, 128, 255 },
	{ 174, 128, 255 },
	{ 186, 128, 255 },
	{ 197, 128, 255 },
	{ 209, 128, 255 },
	{ 220, 128, 255 },
	{ 232, 128, 255 },
	{ 243, 128, 255 },
	{ 255, 128, 255 },
	{ 255, 128, 243 },
	{ 255, 128, 232 },
	{ 255, 128, 220 },
	{ 255, 128, 209 },
	{ 255, 128, 197 },
	{ 255, 128, 186 },
	{ 255, 128, 174 },
	{ 255, 128, 163 },
	{ 255, 128, 151 },
	{ 255, 128, 140 },
	{ 255, 0, 0 },
	{ 255, 23, 0 },
	{ 255, 46, 0 },
	{ 255, 70, 0 },
	{ 255, 93, 0 },
	{ 255, 116, 0 },
	{ 255, 139, 0 },
	{ 255, 162, 0 },
	{ 255, 185, 0 },
	{ 255, 209, 0 },
	{ 255, 232, 0 },
	{ 255, 255, 0 },
	{ 232, 255, 0 },
	{ 209, 255, 0 },
	{ 185, 255, 0 },
	{ 162, 255, 0 },
	{ 139, 255, 0 },
	{ 116, 255, 0 },
	{ 93, 255, 0 },
	{ 70, 255, 0 },
	{ 46, 255, 0 },
	{ 23, 255, 0 },
	{ 0, 255, 0 },
	{ 0, 255, 23 },
	{ 0, 255, 46 },
	{ 0, 255, 70 },
	{ 0, 255, 93 },
	{ 0, 255, 116 },
	{ 0, 255, 139 },
	{ 0, 255, 162 },
	{ 0, 255, 185 },
	{ 0, 255, 209 },
	{ 0, 255, 232 },
	{ 0, 255, 255 },
	{ 0, 232, 255 },
	{ 0, 209, 255 },
	{ 0, 185, 255 },
	{ 0, 162, 255 },
	{ 0, 139, 255 },
	{ 0, 116, 255 },
	{ 0, 93, 255 },
	{ 0, 70, 255 },
	{ 0, 46, 255 },
	{ 0, 23, 255 },
	{ 0, 0, 255 },
	{ 23, 0, 255 },
	{ 46, 0, 255 },
	{ 70, 0, 255 },
	{ 93, 0, 255 },
	{ 116, 0, 255 },
	{ 139, 0, 255 },
	{ 162, 0, 255 },
	{ 185, 0, 255 },
	{ 209, 0, 255 },
	{ 232, 0, 255 },
	{ 255, 0, 255 },
	{ 255, 0, 232 },
	{ 255, 0, 209 },
	{ 255, 0, 185 },
	{ 255, 0, 162 },
	{ 255, 0, 139 },
	{ 255, 0, 116 },
	{ 255, 0, 93 },
	{ 255, 0, 70 },
	{ 255, 0, 46 },
	{ 255, 0, 23 },
	{ 128, 0, 0 },
	{ 128, 12, 0 },
	{ 128, 23, 0 },
	{ 128, 35, 0 },
	{ 128, 47, 0 },
	{ 128, 58, 0 },
	{ 128, 70, 0 },
	{ 128, 81, 0 },
	{ 128, 93, 0 },
	{ 128, 105, 0 },
	{ 128, 116, 0 },
	{ 128, 128, 0 },
	{ 116, 128, 0 },
	{ 105, 128, 0 },
	{ 93, 128, 0 },
	{ 81, 128, 0 },
	{ 70, 128, 0 },
	{ 58, 128, 0 },
	{ 47, 128, 0 },
	{ 35, 128, 0 },
	{ 23, 128, 0 },
	{ 12, 128, 0 },
	{ 0, 128, 0 },
	{ 0, 128, 12 },
	{ 0, 128, 23 },
	{ 0, 128, 35 },
	{ 0, 128, 47 },
	{ 0, 128, 58 },
	{ 0, 128, 70 },
	{ 0, 128, 81 },
	{ 0, 128, 93 },
	{ 0, 128, 105 },
	{ 0, 128, 116 },
	{ 0, 128, 128 },
	{ 0, 116, 128 },
	{ 0, 105, 128 },
	{ 0, 93, 128 },
	{ 0, 81, 128 },
	{ 0, 70, 128 },
	{ 0, 58, 128 },
	{ 0, 47, 128 },
	{ 0, 35, 128 },
	{ 0, 23, 128 },
	{ 0, 12, 128 },
	{ 0, 0, 128 },
	{ 12, 0, 128 },
	{ 23, 0, 128 },
	{ 35, 0, 128 },
	{ 47, 0, 128 },
	{ 58, 0, 128 },
	{ 70, 0, 128 },
	{ 81, 0, 128 },
	{ 93, 0, 128 },
	{ 105, 0, 128 },
	{ 116, 0, 128 },
	{ 128, 0, 128 },
	{ 128, 0, 116 },
	{ 128, 0, 105 },
	{ 128, 0, 93 },
	{ 128, 0, 81 },
	{ 128, 0, 70 },
	{ 128, 0, 58 },
	{ 128, 0, 47 },
	{ 128, 0, 35 },
	{ 128, 0, 23 },
	{ 128, 0, 12 },
	{ 255, 192, 192 },
	{ 255, 64, 64 },
	{ 192, 0, 0 },
	{ 64, 0, 0 },
	{ 255, 255, 192 },
	{ 255, 255, 64 },
	{ 192, 192, 0 },
	{ 64, 64, 0 },
	{ 192, 255, 192 },
	{ 64, 255, 64 },
	{ 0, 192, 0 },
	{ 0, 64, 0 },
	{ 192, 255, 255 },
	{ 64, 255, 255 },
	{ 0, 192, 192 },
	{ 0, 64, 64 },
	{ 192, 192, 255 },
	{ 64, 64, 255 },
	{ 0, 0, 192 },
	{ 0, 0, 64 },
	{ 255, 192, 255 },
	{ 255, 64, 255 },
	{ 192, 0, 192 },
	{ 64, 0, 64 },
	{ 255, 96, 96 },
	{ 255, 255, 255 },
	{ 245, 245, 245 },
	{ 235, 235, 235 },
	{ 224, 224, 224 },
	{ 213, 213, 213 },
	{ 203, 203, 203 },
	{ 192, 192, 192 },
	{ 181, 181, 181 },
	{ 171, 171, 171 },
	{ 160, 160, 160 },
	{ 149, 149, 149 },
	{ 139, 139, 139 },
	{ 128, 128, 128 },
	{ 117, 117, 117 },
	{ 107, 107, 107 },
	{ 96, 96, 96 },
	{ 85, 85, 85 },
	{ 75, 75, 75 },
	{ 64, 64, 64 },
	{ 53, 53, 53 },
	{ 43, 43, 43 },
	{ 32, 32, 32 },
	{ 21, 21, 21 },
	{ 11, 11, 11 },
	{ 0, 0, 0 }
};

static inline uint16_t value_of_2bytes(uint8_t * buf)
{
	return (uint16_t)((buf[0] << 8) | buf[1]);
}

static inline void ilda_push_byte(struct ilda_ctx_t * ctx, struct laserscan_t * l, char byte)
{
	struct ilda_point_t point;

	switch(ctx->state)
	{
	case ILDA_STATE_MAGIC:
		ctx->buf[ctx->index++] = byte;
		if(ctx->index >= 4)
		{
			if(ctx->buf[0] == 'I' && ctx->buf[1] == 'L' && ctx->buf[2] == 'D' && ctx->buf[3] == 'A')
			{
				ctx->state = ILDA_STATE_RESERVED1;
				ctx->index = 0;
			}
			else
			{
				ctx->state = ILDA_STATE_MAGIC;
				ctx->index = 0;
			}
		}
		break;

	case ILDA_STATE_RESERVED1:
		ctx->buf[ctx->index++] = byte;
		if(ctx->index >= 3)
		{
			ctx->state = ILDA_STATE_FORMAT;
			ctx->index = 0;
		}
		break;

	case ILDA_STATE_FORMAT:
		ctx->format = (enum ilda_coord_format_t)byte;
		ctx->state = ILDA_STATE_NAME;
		ctx->index = 0;
		break;

	case ILDA_STATE_NAME:
		ctx->buf[ctx->index++] = byte;
		if(ctx->index >= 8)
		{
			ctx->state = ILDA_STATE_COMPANY;
			ctx->index = 0;
		}
		break;

	case ILDA_STATE_COMPANY:
		ctx->buf[ctx->index++] = byte;
		if(ctx->index >= 8)
		{
			ctx->state = ILDA_STATE_COUNT;
			ctx->index = 0;
		}
		break;

	case ILDA_STATE_COUNT:
		ctx->buf[ctx->index++] = byte;
		if(ctx->index >= 2)
		{
			ctx->count = (int)(value_of_2bytes(&ctx->buf[0]));
			ctx->state = ILDA_STATE_FRAME_NUM;
			ctx->index = 0;
		}
		break;

	case ILDA_STATE_FRAME_NUM:
		ctx->buf[ctx->index++] = byte;
		if(ctx->index >= 2)
		{
			ctx->fnum = (int)(value_of_2bytes(&ctx->buf[0]));
			ctx->state = ILDA_STATE_FRAME_CNT;
			ctx->index = 0;
		}
		break;

	case ILDA_STATE_FRAME_CNT:
		ctx->buf[ctx->index++] = byte;
		if(ctx->index >= 2)
		{
			ctx->fcnt = (int)(value_of_2bytes(&ctx->buf[0]));
			ctx->state = ILDA_STATE_SCANNER;
			ctx->index = 0;
		}
		break;

	case ILDA_STATE_SCANNER:
		ctx->state = ILDA_STATE_RESERVED2;
		ctx->index = 0;
		break;

	case ILDA_STATE_RESERVED2:
		if(ctx->count > 0)
		{
			ctx->record = 0;
			ctx->state = ILDA_STATE_RECORD;
			ctx->index = 0;
		}
		else
		{
			ctx->state = ILDA_STATE_MAGIC;
			ctx->index = 0;
		}
		break;

	case ILDA_STATE_RECORD:
		ctx->buf[ctx->index++] = byte;
		switch(ctx->format)
		{
		case ILDA_COORD_FORMAT_3D_INDEX:
			if(ctx->index >= 8)
			{
				point.x = (int16_t)(value_of_2bytes(&ctx->buf[0])) / 32768.0f;
				point.y = (int16_t)(value_of_2bytes(&ctx->buf[2])) / 32768.0f;
				point.z = (int16_t)(value_of_2bytes(&ctx->buf[4])) / 32768.0f;
				point.index = ctx->buf[7];
				point.r = ilda_default_palette[point.index][0];
				point.g = ilda_default_palette[point.index][1];
				point.b = ilda_default_palette[point.index][2];
				point.a = (ctx->buf[6] && (1 << 6)) ? 0 : 0xff;

				laserscan_set_color(l, point.r, point.g, point.b, point.a);
				laserscan_move_to(l, point.x, point.y);
				ctx->record++;
				ctx->state = ILDA_STATE_RECORD;
				ctx->index = 0;
			}
			break;

		case ILDA_COORD_FORMAT_2D_INDEX:
			if(ctx->index >= 6)
			{
				point.x = (int16_t)(value_of_2bytes(&ctx->buf[0])) / 32768.0f;
				point.y = (int16_t)(value_of_2bytes(&ctx->buf[2])) / 32768.0f;
				point.z = 0;
				point.index = ctx->buf[5];
				point.r = ilda_default_palette[point.index][0];
				point.g = ilda_default_palette[point.index][1];
				point.b = ilda_default_palette[point.index][2];
				point.a = (ctx->buf[4] && (1 << 6)) ? 0 : 0xff;

				laserscan_set_color(l, point.r, point.g, point.b, point.a);
				laserscan_move_to(l, point.x, point.y);
				ctx->record++;
				ctx->state = ILDA_STATE_RECORD;
				ctx->index = 0;
			}
			break;

		case ILDA_COORD_FORMAT_PALETTE_COLOR:
			ctx->state = ILDA_STATE_MAGIC;
			ctx->index = 0;
			break;

		case ILDA_COORD_FORMAT_3D_TRUE_COLOR:
			if(ctx->index >= 10)
			{
				point.x = (int16_t)(value_of_2bytes(&ctx->buf[0])) / 32768.0f;
				point.y = (int16_t)(value_of_2bytes(&ctx->buf[2])) / 32768.0f;
				point.z = (int16_t)(value_of_2bytes(&ctx->buf[4])) / 32768.0f;
				point.b = ctx->buf[7];
				point.g = ctx->buf[8];
				point.r = ctx->buf[9];
				point.a = (ctx->buf[6] && (1 << 6)) ? 0 : 0xff;

				laserscan_set_color(l, point.r, point.g, point.b, point.a);
				laserscan_move_to(l, point.x, point.y);
				ctx->record++;
				ctx->state = ILDA_STATE_RECORD;
				ctx->index = 0;
			}
			break;

		case ILDA_COORD_FORMAT_2D_TRUE_COLOR:
			if(ctx->index >= 8)
			{
				point.x = (int16_t)(value_of_2bytes(&ctx->buf[0])) / 32768.0f;
				point.y = (int16_t)(value_of_2bytes(&ctx->buf[2])) / 32768.0f;
				point.z = 0;
				point.b = ctx->buf[5];
				point.g = ctx->buf[6];
				point.r = ctx->buf[7];
				point.a = (ctx->buf[4] && (1 << 6)) ? 0 : 0xff;

				laserscan_set_color(l, point.r, point.g, point.b, point.a);
				laserscan_move_to(l, point.x, point.y);
				ctx->record++;
				ctx->state = ILDA_STATE_RECORD;
				ctx->index = 0;
			}
			break;

		default:
			break;
		}

		if(ctx->record >= ctx->count)
		{
			ctx->state = ILDA_STATE_MAGIC;
			ctx->index = 0;
		}
		break;

	default:
		ctx->state = ILDA_STATE_MAGIC;
		ctx->index = 0;
		break;
	}
}

void laserscan_load_ilda(struct laserscan_t * l, const char * file)
{
	struct ilda_ctx_t ctx;
	char buf[4096];
	int fd, n, i;

	if(!l || !file)
		return;

	memset(&ctx, 0, sizeof(struct ilda_ctx_t));
	ctx.state = ILDA_STATE_MAGIC;
	ctx.index = 0;

	fd = open(file, O_RDONLY, (S_IRUSR|S_IRGRP|S_IROTH));
	if(fd < 0)
		return;

	while((n = read(fd, buf, sizeof(buf))) > 0)
	{
		for(i = 0; i < n; i++)
		{
			ilda_push_byte(&ctx, l, buf[i]);
		}
	}
	close(fd);
}
