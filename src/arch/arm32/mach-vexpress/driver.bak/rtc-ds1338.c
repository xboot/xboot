/*
 * driver/rtc-ds1338.c
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
#include <i2c/i2c.h>
#include <rtc/rtc.h>

/*
 * Dallas DS1338 - I2C Serial Real-Time Clock
 *
 * Required properties:
 * - i2c-bus: i2c bus name which device attched
 *
 * Optional properties:
 * - square-wave-output: the frequency of square wave on SQW/OUT pin
 *     values usable for ds1338 are:
 *        -1 : high level
 *         0 : low level
 *         1 : 1hz square wave
 *      4096 : 4096hz square wave
 *      8192 : 8192hz square wave
 *     32768 : 32768hz square wave
 *
 * Example:
 *   "rtc-ds1338": {
 *       "i2c-bus": "i2c-versatile.0",
 *       "square-wave-output": 0
 *   }
 */

enum {
	DS1338_SECOND	= 0x0,
	DS1338_MINUTE	= 0x1,
	DS1338_HOUR		= 0x2,
	DS1338_DAY		= 0x3,
	DS1338_DATE		= 0x4,
	DS1338_MONTH	= 0x5,
	DS1338_YEAR		= 0x6,
	DS1338_CTRL		= 0x7,
	DS1338_NVRAM	= 0x8,
};

struct rtc_ds1338_pdata_t {
	struct i2c_device_t * dev;
};

static bool_t ds1338_read(struct i2c_device_t * dev, u8_t reg, u8_t * buf, int len)
{
	struct i2c_msg_t msgs[2];

	msgs[0].addr = dev->addr;
	msgs[0].flags = 0;
	msgs[0].len = 1;
	msgs[0].buf = &reg;

	msgs[1].addr = dev->addr;
	msgs[1].flags = I2C_M_RD;
	msgs[1].len = len;
	msgs[1].buf = buf;

	if(i2c_transfer(dev->i2c, msgs, 2) != 2)
		return FALSE;
	return TRUE;
}

static bool_t ds1338_write(struct i2c_device_t * dev, u8_t reg, u8_t * buf, int len)
{
	struct i2c_msg_t msg;
	u8_t mbuf[256];

	if(len > sizeof(mbuf) - 1)
		len = sizeof(mbuf) - 1;
	mbuf[0] = reg;
	memcpy(&mbuf[1], buf, len);

	msg.addr = dev->addr;
	msg.flags = 0;
	msg.len = len + 1;
	msg.buf = &mbuf[0];

	if(i2c_transfer(dev->i2c, &msg, 1) != 1)
		return FALSE;
	return TRUE;
}

static bool_t rtc_ds1338_settime(struct rtc_t * rtc, struct rtc_time_t * time)
{
	struct rtc_ds1338_pdata_t * pdat = (struct rtc_ds1338_pdata_t *)rtc->priv;
	u8_t buf[7];

	buf[0] = bin2bcd(time->second) & 0x7f;
	buf[1] = bin2bcd(time->minute) & 0x7f;
	buf[2] = bin2bcd(time->hour) & 0x3f;
	buf[3] = bin2bcd(time->week + 1) & 0x7;
	buf[4] = bin2bcd(time->day) & 0x3f;
	buf[5] = bin2bcd(time->month) & 0x1f;
	buf[6] = bin2bcd(time->year - 2000) & 0xff;

	if(ds1338_write(pdat->dev, DS1338_SECOND, buf, 7))
		return TRUE;
	return FALSE;
}

static bool_t rtc_ds1338_gettime(struct rtc_t * rtc, struct rtc_time_t * time)
{
	struct rtc_ds1338_pdata_t * pdat = (struct rtc_ds1338_pdata_t *)rtc->priv;
	u8_t buf[7];

	if(!ds1338_read(pdat->dev, DS1338_SECOND, buf, 7))
		return FALSE;

	time->second = bcd2bin(buf[0] & 0x7f);
	time->minute = bcd2bin(buf[1] & 0x7f);
	time->hour = bcd2bin(buf[2] & 0x3f);
	time->week = bcd2bin(buf[3] & 0x07) - 1;
	time->day = bcd2bin(buf[4] & 0x3f);
	time->month = bcd2bin(buf[5] & 0x1f);
	time->year = bcd2bin(buf[6] & 0xff) + 2000;
	return TRUE;
}

static struct device_t * rtc_ds1338_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct rtc_ds1338_pdata_t * pdat;
	struct rtc_t * rtc;
	struct device_t * dev;
	struct i2c_device_t * i2cdev;
	u8_t buf[8], ctrl;

	i2cdev = i2c_device_alloc(dt_read_string(n, "i2c-bus", NULL), 0x68, 0);
	if(!i2cdev)
		return NULL;

	if(ds1338_read(i2cdev, DS1338_CTRL, &ctrl, 1))
	{
		if(ctrl & 0x20)
		{
			buf[0] = bin2bcd(0) & 0x7f;
			buf[1] = bin2bcd(0) & 0x7f;
			buf[2] = bin2bcd(0) & 0x3f;
			buf[3] = bin2bcd(5 + 1) & 0x7;
			buf[4] = bin2bcd(1) & 0x3f;
			buf[5] = bin2bcd(1) & 0x1f;
			buf[6] = bin2bcd(2016 - 2000) & 0xff;

			switch(dt_read_int(n, "square-wave-output", 0))
			{
			case -1:	/* high level */
				buf[7] = 0x80;
				break;
			case 0:		/* low level */
				buf[7] = 0x00;
				break;
			case 1:		/* 1hz */
				buf[7] = 0x10;
				break;
			case 4096:	/* 4096hz */
				buf[7] = 0x11;
				break;
			case 8192:	/* 8192hz */
				buf[7] = 0x12;
				break;
			case 32768:	/* 32768hz */
				buf[7] = 0x13;
				break;
			default:
				buf[7] = 0x00;
				break;
			}
			ds1338_write(i2cdev, DS1338_SECOND, buf, 8);
		}
	}
	else
	{
		i2c_device_free(i2cdev);
		return NULL;
	}

	pdat = malloc(sizeof(struct rtc_ds1338_pdata_t));
	if(!pdat)
	{
		i2c_device_free(i2cdev);
		return NULL;
	}

	rtc = malloc(sizeof(struct rtc_t));
	if(!rtc)
	{
		i2c_device_free(i2cdev);
		free(pdat);
		return NULL;
	}

	pdat->dev = i2cdev;

	rtc->name = alloc_device_name(dt_read_name(n), -1);
	rtc->settime = rtc_ds1338_settime;
	rtc->gettime = rtc_ds1338_gettime;
	rtc->priv = pdat;

	if(!register_rtc(&dev, rtc))
	{
		i2c_device_free(pdat->dev);

		free_device_name(rtc->name);
		free(rtc->priv);
		free(rtc);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void rtc_ds1338_remove(struct device_t * dev)
{
	struct rtc_t * rtc = (struct rtc_t *)dev->priv;
	struct rtc_ds1338_pdata_t * pdat = (struct rtc_ds1338_pdata_t *)rtc->priv;

	if(rtc && unregister_rtc(rtc))
	{
		i2c_device_free(pdat->dev);

		free_device_name(rtc->name);
		free(rtc->priv);
		free(rtc);
	}
}

static void rtc_ds1338_suspend(struct device_t * dev)
{
}

static void rtc_ds1338_resume(struct device_t * dev)
{
}

static struct driver_t rtc_ds1338 = {
	.name		= "rtc-ds1338",
	.probe		= rtc_ds1338_probe,
	.remove		= rtc_ds1338_remove,
	.suspend	= rtc_ds1338_suspend,
	.resume		= rtc_ds1338_resume,
};

static __init void rtc_ds1338_driver_init(void)
{
	register_driver(&rtc_ds1338);
}

static __exit void rtc_ds1338_driver_exit(void)
{
	unregister_driver(&rtc_ds1338);
}

driver_initcall(rtc_ds1338_driver_init);
driver_exitcall(rtc_ds1338_driver_exit);
