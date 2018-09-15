/*
 * driver/ts-gt911.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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
#include <i2c/i2c.h>
#include <gpio/gpio.h>
#include <interrupt/interrupt.h>
#include <input/input.h>

#include <cairo-xboot.h>
//#include <framebuffer/framebuffer.h>

#define CFG_VERSION 0x4780
#define PRODUCT_ID  0x4081
#define State_ADD   0x4E81
#define COOR_ADD    0x4F81
#define CFG911_LEN	186

struct ts_gt911_pdata_t {
	struct i2c_device_t * dev;
	int irq;
	u8_t fingers;
	struct {
		int x, y;
		int press;
		int count;
		char change;
		int valid;
	} node[10];
};

int oldx,oldy;
int touch_count=0,count_max=2;//for touch frame jump

static int gt911_read(struct i2c_device_t * dev, u16_t reg, u8_t * buf, int len)
{
	struct i2c_msg_t msgs[2];
	int ret;

    msgs[0].addr = dev->addr;
    msgs[0].flags = 0;
    msgs[0].len = 2;
    msgs[0].buf = &reg;

    msgs[1].addr = dev->addr;
    msgs[1].flags = I2C_M_RD;
    msgs[1].len = len;
    msgs[1].buf = buf;

    ret=i2c_transfer(dev->i2c, msgs, 2);//this must 2

    return ret;// < 0 ? ret : (ret != 2 ? -EIO : 0);
}

static bool_t gt911_write(struct i2c_device_t * dev, u16_t reg, u8_t * buf, int len)
{
	struct i2c_msg_t msg;
	u8_t mbuf[256];

	if(len > sizeof(mbuf) - 1)
		len = sizeof(mbuf) - 1;
	mbuf[0] = reg & 0xff;
	mbuf[1] = (reg >> 8) & 0xff;
	memcpy(&mbuf[2], buf, len);

    msg.addr = dev->addr;
    msg.flags = 0;
    msg.len = len + 2;
    msg.buf = &mbuf[0];

    if(i2c_transfer(dev->i2c, &msg, 1) != 1)
    	return FALSE;
    return TRUE;
}

static int goodix_i2c_test(struct i2c_device_t * dev)
{
	int ret;
	int retry=0;
	u8_t test;

	while (retry++ < 2)
	{
		ret = gt911_read(dev, CFG_VERSION, &test, 1);
		if(ret == 2)
		{
			LOG("i2c test CFG_VER:%c",test);
			return 1;
		}
		LOG("i2c test failed attempt %d: %d",retry,test);
		mdelay(20);
	}
	return 0;
}

static int goodix_read_version(struct i2c_device_t * dev)
{
	int error;
	u8_t buf[6];
	//char id_str[6];
	//u16_t gt_version;

	error = gt911_read(dev, PRODUCT_ID, buf, sizeof(buf));
	if (error !=2) {
		LOG("read id fail! error:%d",error);
		return error;
	}

	//memcpy(id_str, buf, 6);
	//id_str[4] = 0;

	//gt_version = get_unaligned_le16(&buf[4]);

	LOG("GT911 verson: %c%c%c%c_%02x%02x",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5]);

	return 0;
}

static int goodix_send_cfg(struct i2c_device_t * dev)
{
	int i;
	u8_t check_sum = 0;
	u8_t ret;

	/*u8_t config0[CFG911_LEN]={//one point
			0x41,0x20,0x03,0xE0,0x01,0x01,0x3C,0x00,0x01,0x08,
			0x19,0x05,0x4A,0x3A,0x03,0x0F,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x18,0x1A,0x1E,0x14,0x89,0x2A,0x09,
			0x2F,0x2B,0x88,0x13,0x00,0x00,0x01,0xB8,0x03,0x2D,
			0x00,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x02,0x04,0x06,0x08,0x0A,0x0C,0x10,0x12,
			0x14,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x02,0x04,0x06,0x08,0x0A,0x0F,0x10,
			0x12,0x16,0x18,0x1C,0x1D,0x1E,0x1F,0x20,0x21,0x22,
			0x24,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0xF2,0x01
			};*/
	u8_t config0[CFG911_LEN]={//tow point
			0x41,0x20,0x03,0xE0,0x01,0x02,0x3C,0x00,0x01,0x08,
			0x19,0x05,0x4A,0x3A,0x03,0x0F,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x18,0x1A,0x1E,0x14,0x89,0x2A,0x09,
			0x2F,0x2B,0x88,0x13,0x00,0x00,0x01,0xB8,0x03,0x2D,
			0x00,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x02,0x04,0x06,0x08,0x0A,0x0C,0x10,0x12,
			0x14,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x02,0x04,0x06,0x08,0x0A,0x0F,0x10,
			0x12,0x16,0x18,0x1C,0x1D,0x1E,0x1F,0x20,0x21,0x22,
			0x24,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0xF1,0x01
			};
	for (i = 0; i < (CFG911_LEN-2); i++)
		check_sum += config0[i];
	check_sum = (~check_sum) + 1;
	if (check_sum != config0[CFG911_LEN-2]) {
		LOG("check_sum err:%02x",check_sum);
		return 0;
	}

	if (config0[CFG911_LEN - 1] != 1) {
		LOG("cfg err:last byte must be 0x01");
		return 0;
	}

	ret = gt911_write(dev, CFG_VERSION, config0, CFG911_LEN);
	if(ret)//no err
	{
		LOG("send cfg ok");
		return 0;
	}else
	{
		LOG("send cfg err");
		return 1;
	}
}

void draw_line(int x, int y)
{
	struct device_t * pos, * n;
	//cairo_surface_t * logo;
	cairo_surface_t * cs;
	cairo_t * cr;
	struct framebuffer_t * fb;

	list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_FRAMEBUFFER], head)
	{
		if((fb = (struct framebuffer_t *)(pos->priv)))
		{
			cs = cairo_xboot_surface_create(fb, fb->alone);
			cr = cairo_create(cs);

			cairo_set_source_rgb(cr, 0, 0, 0);
			cairo_set_line_width(cr,1);

			cairo_move_to(cr,oldx,oldy);
			cairo_line_to(cr,x,y);
			cairo_stroke(cr);

			cairo_destroy(cr);
			cairo_xboot_surface_present(cs);
			cairo_surface_destroy(cs);

			//framebuffer_set_backlight(fb, CONFIG_MAX_BRIGHTNESS);
		}
	}
}

static void gt911_interrupt(void * data)
{
	struct input_t * input = (struct input_t *)data;
	struct ts_gt911_pdata_t * pdat = (struct ts_gt911_pdata_t *)input->priv;
	u8_t max_fingers=pdat->fingers;
	u8_t buf[40];
	u8_t id;
	int x, y;
	u8_t touch_points, i;
	u8_t point_status;
	int ret;

	//LOG("interrupt");
	disable_irq(pdat->irq);
	ret = gt911_read(pdat->dev, State_ADD, buf, 1);
	if(ret !=2)
		LOG("int read status err:%d",ret);
	point_status=buf[0];
	//LOG("point_status:%02x",point_status);
	if((point_status>>7) & 1)
		touch_points=point_status & 0x0f;
	else
		touch_points=0;
	//LOG("touch_points:%d",touch_points);
	if(touch_points>0)
	{
		ret = gt911_read(pdat->dev, COOR_ADD, buf, touch_points*8);
		if(ret !=2)
			LOG("int read coord err:%d",ret);
	}

	for(i = 0; i < max_fingers; i++)
	{
		pdat->node[i].valid = 0;
		//pdat->node[i].change = 0;
	}

	for(i = 0; i < touch_points; i++)
	{
		id = buf[i*8];
		x = buf[2+i*8]<<8 | buf[1+i*8];
		y = buf[4+i*8]<<8 | buf[3+i*8];
		//if(pdat->node[id].x != x || pdat->node[id].y != y)
		//{
			if(pdat->node[id].press == 0)
			{
				push_event_touch_begin(input, x, y, id);
				pdat->node[id].press = 1;
				pdat->node[id].count=0;
				//touch_count=0;
				//oldx=x;
				//oldy=y;
				//LOG("T%d x%d: y:%d",id,x,y);
			}
			else if(pdat->node[id].press == 1)
			{
				//touch_count++;
				pdat->node[id].count++;
				//if(pdat->node[id].count==count_max)
				{
					/*if(i==0)
						push_event_touch_move2(input, x, y, id,touch_points);
					else
						push_event_touch_move2(input, x, y, id,0);*/
					//push_event_touch_move2(input, x, y, id,0,1);
					push_event_touch_move(input, x, y, id);
					//pdat->node[id].change=1;
					pdat->node[id].count=0;
				}
				//draw_line(x,y);
				//oldx=x;
				//oldy=y;
				//LOG("T%d x%d: y:%d",id,x,y);
			}
		//}
		pdat->node[id].x = x;
		pdat->node[id].y = y;
		pdat->node[id].valid = 1;
	}
	/*if(pdat->node[0].change && pdat->node[1].change)
	{
		push_event_touch_move2(input, pdat->node[0].x, pdat->node[0].y, 0,1,touch_points);//for continue pump
		push_event_touch_move2(input, pdat->node[1].x, pdat->node[1].y, 1,0,touch_points);
	}else if(pdat->node[0].change)
	{
		push_event_touch_move2(input, pdat->node[0].x, pdat->node[0].y, 0,0,touch_points);
	}else if(pdat->node[1].change)
	{
		push_event_touch_move2(input, pdat->node[1].x, pdat->node[1].y, 1,0,touch_points);
	}*/

	for(i = 0; i < max_fingers; i++)
	{
		if((pdat->node[i].press == 1) && (pdat->node[i].valid == 0))
		{
			push_event_touch_end(input, pdat->node[i].x, pdat->node[i].y, i);
			pdat->node[i].press = 0;
			//LOG("T%d x%d: y:%d",i,pdat->node[i].x,pdat->node[i].y);
		}
	}

	if((point_status>>7) & 1)
	{
		point_status=0;
		ret = gt911_write(pdat->dev, State_ADD, &point_status, 1);
		if(!ret)
			LOG("clear status err:%d",ret);
	}

	enable_irq(pdat->irq);
}

static int ts_gt911_ioctl(struct input_t * input, int cmd, void * arg)
{
	return -1;
}

static struct device_t * ts_gt911_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct ts_gt911_pdata_t * pdat;
	struct input_t * input;
	struct device_t * dev;
	struct i2c_device_t * i2cdev;
	int error;
	int int_gpio,irq,rst_gpio;

	int_gpio = dt_read_int(n, "interrupt-gpio", -1);
	if(!gpio_is_valid(int_gpio))
	{
		LOG("int_gpio is not valid!");
		return NULL;
	}
	irq = gpio_to_irq(int_gpio);
	LOG("int_irq:%d",irq);
	if(!irq_is_valid(irq))
	{
		LOG("irq is not valid!");
		return NULL;
	}
	rst_gpio = dt_read_int(n, "reset-gpio", -1);
	if(!gpio_is_valid(rst_gpio))
	{
		LOG("rst_gpio is not valid!");
		return NULL;
	}

	//reset and select i2c addr
	gpio_set_direction(rst_gpio, GPIO_DIRECTION_OUTPUT);
	gpio_set_value(rst_gpio, 0);
	mdelay(20);//T2>=10ms
	//HIGH: 0x14, LOW: 0x5d
	gpio_set_direction(int_gpio, GPIO_DIRECTION_OUTPUT);
	gpio_set_value(int_gpio, 1);
	udelay(150);//T3>=100us
	gpio_set_value(rst_gpio, 1);
	mdelay(8);//T4>=5ms
	gpio_set_pull(int_gpio, GPIO_PULL_DOWN);
	gpio_set_direction(int_gpio, GPIO_DIRECTION_INPUT);
	//end select

	i2cdev = i2c_device_alloc(dt_read_string(n, "i2c-bus", NULL), 0x14, 0);//0x14 is addr
	if(!i2cdev)
	{
		LOG("i2c_device_alloc fail!");
		return NULL;
	}

	if(!goodix_i2c_test(i2cdev))
	{
		return NULL;
	}

	error=goodix_read_version(i2cdev);
	if(error)
		return NULL;

	mdelay(50);//maybe need 50ms before send config
	error=goodix_send_cfg(i2cdev);
	if(error)
		return NULL;
	mdelay(10);//Let the firmware reconfigure itself

	pdat = malloc(sizeof(struct ts_gt911_pdata_t));
	if(!pdat)
	{
		i2c_device_free(i2cdev);
		return NULL;
	}

	input = malloc(sizeof(struct input_t));
	if(!input)
	{
		i2c_device_free(i2cdev);
		free(pdat);
		return NULL;
	}

	memset(pdat, 0, sizeof(struct ts_gt911_pdata_t));
	pdat->dev = i2cdev;
	pdat->irq = irq;
	pdat->fingers = dt_read_int(n, "maximum-fingers", 5);

	input->name = alloc_device_name(dt_read_name(n), -1);
	input->type = INPUT_TYPE_TOUCHSCREEN;
	input->ioctl = ts_gt911_ioctl;
	input->priv = pdat;

	gpio_set_cfg(int_gpio,6);//use EINTE3
	//LOG("gt911 request_irq");
	error=request_irq(pdat->irq, gt911_interrupt, IRQ_TYPE_EDGE_RISING, input);
	if(!error)
	{
		LOG("request_irq fail");
		return NULL;
	}


	if(!register_input(&dev, input))
	{
		free_irq(pdat->irq);
		i2c_device_free(pdat->dev);

		free_device_name(input->name);
		free(input->priv);
		free(input);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void ts_gt911_remove(struct device_t * dev)
{
	struct input_t * input = (struct input_t *)dev->priv;
	struct ts_gt911_pdata_t * pdat = (struct ts_gt911_pdata_t *)input->priv;

	if(input && unregister_input(input))
	{
		free_irq(pdat->irq);
		i2c_device_free(pdat->dev);

		free_device_name(input->name);
		free(input->priv);
		free(input);
	}
}

static void ts_gt911_suspend(struct device_t * dev)
{
}

static void ts_gt911_resume(struct device_t * dev)
{
}

static struct driver_t ts_gt911 = {
	.name		= "ts-GT911",
	.probe		= ts_gt911_probe,
	.remove		= ts_gt911_remove,
	.suspend	= ts_gt911_suspend,
	.resume		= ts_gt911_resume,
};

static __init void ts_gt911_driver_init(void)
{
	register_driver(&ts_gt911);
}

static __exit void ts_gt911_driver_exit(void)
{
	unregister_driver(&ts_gt911);
}

driver_initcall(ts_gt911_driver_init);
driver_exitcall(ts_gt911_driver_exit);
