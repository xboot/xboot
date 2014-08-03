/*
 * driver/ft5206.c
 *
 * Copyright(c) 2007-2014 Jianjun Jiang <8192542@qq.com>
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

#include <xboot.h>
#include <ft5206.h>

#define S5PV210_EXT_INT0_CON 	 		(0xE0200000 + 0x0E00)
#define S5PV210_EXT_INT0_MASK 	 		(0xE0200000 + 0x0F00)
#define S5PV210_EXT_INT0_PEND 	 		(0xE0200000 + 0x0F40)

enum {
	FT5206_REG_THGROUP					= 0x80,
	FT5206_REG_THPEAK					= 0x81,
	FT5206_REG_THCAL					= 0x82,
	FT5206_REG_THWATER					= 0x83,
	FT5206_REG_THTEMP					= 0x84,
	FT5206_REG_THDIFF					= 0x85,
	FT5206_REG_CTRL						= 0x86,
	FT5206_REG_TIMEENTERMONITOR			= 0x87,
	FT5206_REG_PERIODACTIVE				= 0x88,
	FT5206_REG_PERIODMONITOR			= 0x89,
	FT5206_REG_HEIGHT_B					= 0x8a,
	FT5206_REG_MAX_FRAME				= 0x8b,
	FT5206_REG_DIST_MOVE				= 0x8c,
	FT5206_REG_DIST_POINT				= 0x8d,
	FT5206_REG_FEG_FRAME				= 0x8e,
	FT5206_REG_SINGLE_CLICK_OFFSET		= 0x8f,
	FT5206_REG_DOUBLE_CLICK_TIME_MIN	= 0x90,
	FT5206_REG_SINGLE_CLICK_TIME		= 0x91,
	FT5206_REG_LEFT_RIGHT_OFFSET		= 0x92,
	FT5206_REG_UP_DOWN_OFFSET			= 0x93,
	FT5206_REG_DISTANCE_LEFT_RIGHT		= 0x94,
	FT5206_REG_DISTANCE_UP_DOWN			= 0x95,
	FT5206_REG_ZOOM_DIS_SQR				= 0x96,
	FT5206_REG_RADIAN_VALUE				= 0x97,
	FT5206_REG_MAX_X_HIGH				= 0x98,
	FT5206_REG_MAX_X_LOW             	= 0x99,
	FT5206_REG_MAX_Y_HIGH            	= 0x9a,
	FT5206_REG_MAX_Y_LOW             	= 0x9b,
	FT5206_REG_K_X_HIGH            		= 0x9c,
	FT5206_REG_K_X_LOW             		= 0x9d,
	FT5206_REG_K_Y_HIGH            		= 0x9e,
	FT5206_REG_K_Y_LOW             		= 0x9f,
	FT5206_REG_AUTO_CLB_MODE			= 0xa0,
	FT5206_REG_LIB_VERSION_H 			= 0xa1,
	FT5206_REG_LIB_VERSION_L 			= 0xa2,
	FT5206_REG_CIPHER					= 0xa3,
	FT5206_REG_MODE						= 0xa4,
	FT5206_REG_PMODE					= 0xa5,
	FT5206_REG_FIRMID					= 0xa6,
	FT5206_REG_STATE					= 0xa7,
	FT5206_REG_FT5201ID					= 0xa8,
	FT5206_REG_ERR						= 0xa9,
	FT5206_REG_CLB						= 0xaa,
};

struct ft5206_private_data_t {
	struct {
		int x, y;
		int event;
	} node[5];
	struct i2c_client_t * client;
	struct ft5206_data_t * rdat;
};

static bool_t ft5206_reset(struct ft5206_data_t * rdat)
{
	gpio_set_pull(rdat->rst_pin, GPIO_PULL_UP);
	gpio_direction_output(rdat->rst_pin, 0);
	mdelay(10);

	gpio_set_value(rdat->rst_pin, 1);
	mdelay(100);

	return TRUE;
}

static bool_t ft5206_read_reg(struct i2c_client_t * client, u8_t reg, u8_t * val)
{
	struct i2c_msg_t msgs[2];
    u8_t buf;

    buf = reg;
    msgs[0].addr = client->addr;
    msgs[0].flags = 0;
    msgs[0].len = 1;
    msgs[0].buf = &buf;

    msgs[1].addr = client->addr;
    msgs[1].flags = I2C_M_RD;
    msgs[1].len = 1;
    msgs[1].buf = &buf;

    if(i2c_transfer(client->i2c, msgs, 2) != 2)
    	return FALSE;

    if(val)
    	*val = buf;
    return TRUE;
}

static bool_t ft5206_read(struct i2c_client_t * client, u8_t * buf, u8_t len)
{
	struct i2c_msg_t msgs[2];
	u8_t mbuf = 0;

	msgs[0].flags = 0;
	msgs[0].addr = client->addr;
	msgs[0].len = 1;
	msgs[0].buf = &mbuf;

	msgs[1].flags = I2C_M_RD;
	msgs[1].addr = client->addr;
	msgs[1].len = len;
	msgs[1].buf = buf;

    if(i2c_transfer(client->i2c, msgs, 2) != 2)
    	return FALSE;
    return TRUE;
}

static void ft5206_interrupt_function(void * data)
{
	struct input_t * input = (struct input_t *)data;
	struct ft5206_private_data_t * dat = (struct ft5206_private_data_t *)input->priv;
	u8_t buf[32];
	int X, Y, x, y, event, id;
	int i;

	if(ft5206_read(dat->client, buf, sizeof(buf)))
	{
		for(i = 0; i < 5; i++)
		{
			X = (buf[i * 6 + 3]) << 8 | buf[i * 6 + 4];
			Y = (buf[i * 6 + 5]) << 8 | buf[i * 6 + 6];

			x = X & 0xfff;
			y = Y & 0xfff;
			y = 480 - y;

			event = (X >> 14) & 0x3;
			id = ((Y >> 12) & 0xf);

			if(id >= 0 && id <= 4)
			{
				if(dat->node[id].x != x || dat->node[id].y != y || dat->node[i].event != event)
				{
					if(event == 0)
					{
						push_event_touches_begin(input, x, y, id);
						//LOG("[%d]down: x = %4d, y = %4d", id, x, y);
					}
					else if(event == 0x2)
					{
						push_event_touches_move(input, x, y, id);
						//LOG("[%d]move: x = %4d, y = %4d", id, x, y);
					}
					else if(event == 0x1)
					{
						push_event_touches_end(input, x, y, id);
						//LOG("[%d]up: x = %4d, y = %4d", id, x, y);
					}

					dat->node[id].x = x;
					dat->node[id].y = y;
					dat->node[id].event = event;
				}
			}
		}
	}

	writel(S5PV210_EXT_INT0_PEND, (readl(S5PV210_EXT_INT0_PEND) & ~(0x1<<7)) | (0x1<<7));
}

static void input_init(struct input_t * input)
{
	gpio_set_cfg(S5PV210_GPH0(7), 0xf);
	gpio_set_pull(S5PV210_GPH0(7), GPIO_PULL_NONE);
	writel(S5PV210_EXT_INT0_CON, (readl(S5PV210_EXT_INT0_CON) & ~(0x7<<28)) | (0x4<<28));
	writel(S5PV210_EXT_INT0_MASK, (readl(S5PV210_EXT_INT0_MASK) & ~(0x1<<7)) | (0x0<<7));

	if(!request_irq("EINT7", ft5206_interrupt_function, input))
		LOG("Can't request irq 'EINT7'");
}

static void input_exit(struct input_t * input)
{
	if(!free_irq("EINT7"))
		LOG("Can't free irq 'EINT7'");
}

static int input_ioctl(struct input_t * input, int cmd, void * arg)
{
	return -1;
}

static void input_suspend(struct input_t * input)
{
}

static void input_resume(struct input_t * input)
{
}

static bool_t register_ft5206_touchscreen(struct resource_t * res)
{
	struct ft5206_data_t * rdat = (struct ft5206_data_t *)res->data;
	struct ft5206_private_data_t * dat;
	struct input_t * input;
	struct i2c_client_t * client;
	char name[64];
	u8_t val;

	if(!ft5206_reset(rdat))
		return FALSE;

	client = i2c_client_alloc(rdat->i2cbus, rdat->addr, 0);
	if(!client)
		return FALSE;

	if(ft5206_read_reg(client, FT5206_REG_FIRMID, &val))
	{
		LOG("FT5206 firmware version 0x%x", val);
	}
	else
	{
		i2c_client_free(client);
		LOG("FT5206 not found, please check!");
		return FALSE;
	}

	if(ft5206_read_reg(client, FT5206_REG_PERIODACTIVE, &val))
		LOG("FT5206 report rate %dHZ", val * 10);

    if(ft5206_read_reg(client, FT5206_REG_THGROUP, &val))
    	LOG("FT5206 touch threshold %d", val * 4);

	dat = malloc(sizeof(struct ft5206_private_data_t));
	if(!dat)
	{
		i2c_client_free(client);
		return FALSE;
	}

	input = malloc(sizeof(struct input_t));
	if(!input)
	{
		i2c_client_free(client);
		free(dat);
		return FALSE;
	}

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	memset(dat, 0xff, sizeof(struct ft5206_private_data_t));
	dat->client = client;
	dat->rdat = rdat;

	input->name = strdup(name);
	input->type = INPUT_TYPE_TOUCHSCREEN;
	input->init = input_init;
	input->exit = input_exit;
	input->ioctl = input_ioctl;
	input->suspend = input_suspend,
	input->resume = input_resume,
	input->priv = dat;

	if(register_input(input))
		return TRUE;

	i2c_client_free(client);
	free(input->priv);
	free(input->name);
	free(input);
	return FALSE;
}

static bool_t unregister_ft5206_touchscreen(struct resource_t * res)
{
	struct input_t * input;
	char name[64];

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	input = search_input(name);
	if(!input)
		return FALSE;

	if(!unregister_input(input))
		return FALSE;

	i2c_client_free(((struct ft5206_private_data_t *)(input->priv))->client);
	free(input->priv);
	free(input->name);
	free(input);
	return TRUE;
}

static __init void ft5206_device_init(void)
{
	resource_for_each_with_name("ft5206", register_ft5206_touchscreen);
}

static __exit void ft5206_device_exit(void)
{
	resource_for_each_with_name("ft5206", unregister_ft5206_touchscreen);
}

device_initcall(ft5206_device_init);
device_exitcall(ft5206_device_exit);
