/*
 * driver/gslx680.c
 *
 * Copyright(c) 2007-2015 Jianjun Jiang <8192542@qq.com>
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
#include <gslx680.h>

struct gslx680_private_data_t {
	struct i2c_client_t * client;
	struct gslx680_data_t * rdat;
};

static bool_t gslx680_read(struct i2c_client_t * client, u8_t reg, u8_t * buf, u32_t len)
{
	struct i2c_msg_t msgs[2];

    msgs[0].addr = client->addr;
    msgs[0].flags = 0;
    msgs[0].len = 1;
    msgs[0].buf = &reg;

    msgs[1].addr = client->addr;
    msgs[1].flags = I2C_M_RD;
    msgs[1].len = len;
    msgs[1].buf = buf;

    if(i2c_transfer(client->i2c, msgs, 2) != 2)
    	return FALSE;
    return TRUE;
}

static bool_t gslx680_write(struct i2c_client_t * client, u8_t reg, u8_t * buf, u32_t len)
{
	struct i2c_msg_t msg;
	u8_t mbuf[256];

	if(len > sizeof(mbuf) - 1)
		len = sizeof(mbuf) - 1;
	mbuf[0] = reg;
	memcpy(&mbuf[1], buf, len);

    msg.addr = client->addr;
    msg.flags = 0;
    msg.len = len + 1;
    msg.buf = &mbuf[0];

    if(i2c_transfer(client->i2c, &msg, 1) != 1)
    	return FALSE;
    return TRUE;
}

static bool_t gslx680_wakeup_pin(int pin, int high)
{
	if(high)
		gpio_direction_output(pin, 1);
	else
		gpio_direction_output(pin, 0);
	return TRUE;
}

static bool_t gslx680_check(struct i2c_client_t * client)
{
	u8_t buf;

	buf = 0x12;
	if(!gslx680_write(client, 0xf0, &buf, 1))
		return FALSE;

	buf = 0x00;
	if(!gslx680_read(client, 0xf0, &buf, 1))
		return FALSE;

	if(buf == 0x12)
		return TRUE;
	return FALSE;
}

static void gslx680_clear(struct i2c_client_t * client)
{
	u8_t buf;

	buf = 0x88;
	gslx680_write(client, 0xe0, &buf, 1);
	mdelay(20);

	buf = 0x03;
	gslx680_write(client, 0x80, &buf, 1);
	mdelay(5);

	buf = 0x04;
	gslx680_write(client, 0xe4, &buf, 1);
	mdelay(5);

	buf = 0x00;
	gslx680_write(client, 0xe0, &buf, 1);
	mdelay(20);
}

static bool_t gslx680_reset(struct i2c_client_t * client)
{
	u8_t buf;

	buf = 0x88;
	gslx680_write(client, 0xe0, &buf, 1);
	mdelay(20);

	buf = 0x04;
	gslx680_write(client, 0xe4, &buf, 1);
	mdelay(10);

	buf = 0x00;
	gslx680_write(client, 0xbc, &buf, 1);
	mdelay(10);

	buf = 0x00;
	gslx680_write(client, 0xbd, &buf, 1);
	mdelay(10);

	buf = 0x00;
	gslx680_write(client, 0xbe, &buf, 1);
	mdelay(10);

	buf = 0x00;
	gslx680_write(client, 0xbf, &buf, 1);
	mdelay(10);

	return TRUE;
}

static bool_t gslx680_startup(struct i2c_client_t * client)
{
	u8_t buf;

	buf = 0x00;
	gslx680_write(client, 0xe0, &buf, 1);
	mdelay(10);
	return TRUE;
}

static bool_t gslx680_load_firmware(struct i2c_client_t * client, const struct gslx680_firmware_t * fw)
{
	struct gslx680_firmware_t * next = (struct gslx680_firmware_t *)fw;
	u8_t buf[4];

	if(!next)
		return FALSE;

	while(!((next->reg == 0xFF) && (next->val == 0xFFFFFFFF)))
	{
		buf[0] = ((next->val) >> 0) & 0xff;
		buf[1] = ((next->val) >> 8) & 0xff;
		buf[2] = ((next->val) >> 16) & 0xff;
		buf[3] = ((next->val) >> 24) & 0xff;

		if(!gslx680_write(client, next->reg, buf, 4))
			return FALSE;
		next++;
	}

	return TRUE;
}

static void gslx680_interrupt_function(void * data)
{
	struct input_t * input = (struct input_t *)data;
	struct gslx680_private_data_t * dat = (struct gslx680_private_data_t *)input->priv;
	u8_t buf[4+4];

	disable_irq(dat->rdat->irq);

	if(!gslx680_read(dat->client, 0x80, &buf[0], 8))
		return;

	printf("f = 0x%02x, 0x%02x, 0x%02x, 0x%02x, \r\n", buf[0], buf[1], buf[2], buf[3]);
//	printf("t = 0x%02x, 0x%02x, 0x%02x, 0x%02x, \r\n", buf[4], buf[5], buf[6], buf[7]);

	enable_irq(dat->rdat->irq);
}

static void input_init(struct input_t * input)
{
	struct gslx680_private_data_t * dat = (struct gslx680_private_data_t *)input->priv;
	request_irq(dat->rdat->irq, gslx680_interrupt_function, IRQ_TYPE_EDGE_RISING, input);
}

static void input_exit(struct input_t * input)
{
	struct gslx680_private_data_t * dat = (struct gslx680_private_data_t *)input->priv;
	free_irq(dat->rdat->irq);
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

static bool_t register_gslx680_touchscreen(struct resource_t * res)
{
	struct gslx680_data_t * rdat = (struct gslx680_data_t *)res->data;
	struct gslx680_private_data_t * dat;
	struct input_t * input;
	struct i2c_client_t * client;
	char name[64];

	client = i2c_client_alloc(rdat->i2cbus, rdat->addr, 0);
	if(!client)
		return FALSE;

	if(rdat->wakepin >= 0)
	{
		gslx680_wakeup_pin(rdat->wakepin, 0);
		mdelay(20);
		gslx680_wakeup_pin(rdat->wakepin, 1);
		mdelay(20);
	}

	if(!gslx680_check(client))
	{
		i2c_client_free(client);
		return FALSE;
	}

	gslx680_clear(client);
	gslx680_reset(client);
	gslx680_load_firmware(client, rdat->firmware);
	gslx680_startup(client);
	gslx680_reset(client);
	gslx680_startup(client);
	LOG("Found gslx680 chip and load firmware");

	dat = malloc(sizeof(struct gslx680_private_data_t));
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

	memset(dat, 0, sizeof(struct gslx680_private_data_t));
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

static bool_t unregister_gslx680_touchscreen(struct resource_t * res)
{
	struct input_t * input;
	char name[64];

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	input = search_input(name);
	if(!input)
		return FALSE;

	if(!unregister_input(input))
		return FALSE;

	i2c_client_free(((struct gslx680_private_data_t *)(input->priv))->client);
	free(input->priv);
	free(input->name);
	free(input);
	return TRUE;
}

static __init void gslx680_device_init(void)
{
	resource_for_each_with_name("gslx680", register_gslx680_touchscreen);
}

static __exit void gslx680_device_exit(void)
{
	resource_for_each_with_name("gslx680", unregister_gslx680_touchscreen);
}

device_initcall(gslx680_device_init);
device_exitcall(gslx680_device_exit);
