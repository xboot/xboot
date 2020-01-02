/*
 * driver/rtc-sd3068.c
 *
 * Copyright(c) 2007-2020 Jianjun Jiang <8192542@qq.com>
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

enum {
	SD3068_SECOND	= 0x00,
	SD3068_MINUTE	= 0x01,
	SD3068_HOUR		= 0x02,
	SD3068_DAY		= 0x03,
	SD3068_DATE		= 0x04,
	SD3068_MONTH	= 0x05,
	SD3068_YEAR		= 0x06,
	SD3068_CTR1		= 0x0f,
	SD3068_CTR2		= 0x10,
	SD3068_CTR3		= 0x11,
	SD3068_ID		= 0x72,
};

struct rtc_sd3068_pdata_t {
	struct i2c_device_t * dev;
};

static bool_t sd3068_read(struct i2c_device_t * dev, u8_t reg, u8_t * buf, int len)
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

static bool_t sd3068_write(struct i2c_device_t * dev, u8_t reg, u8_t * buf, int len)
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

static bool_t rtc_sd3068_settime(struct rtc_t * rtc, struct rtc_time_t * time)
{
	struct rtc_sd3068_pdata_t * pdat = (struct rtc_sd3068_pdata_t *)rtc->priv;
	u8_t buf[7], val;

	val = 0xff;
	sd3068_write(pdat->dev, SD3068_CTR2, &val, 1);
	val = 0x80;
	sd3068_write(pdat->dev, SD3068_CTR1, &val, 1);

	buf[0] = bin2bcd(time->second) & 0x7f;
	buf[1] = bin2bcd(time->minute) & 0x7f;
	buf[2] = bin2bcd(time->hour) & 0x3f;
	buf[3] = bin2bcd(time->week + 1) & 0x7;
	buf[4] = bin2bcd(time->day) & 0x3f;
	buf[5] = bin2bcd(time->month) & 0x1f;
	buf[6] = bin2bcd(time->year - 2000) & 0xff;
	sd3068_write(pdat->dev, SD3068_SECOND, buf, 7);

	val = 0x7b;
	sd3068_write(pdat->dev, SD3068_CTR1, &val, 1);
	val = 0x00;
	sd3068_write(pdat->dev, SD3068_CTR2, &val, 1);

	return TRUE;
}

static bool_t rtc_sd3068_gettime(struct rtc_t * rtc, struct rtc_time_t * time)
{
	struct rtc_sd3068_pdata_t * pdat = (struct rtc_sd3068_pdata_t *)rtc->priv;
	u8_t buf[7];

	if(!sd3068_read(pdat->dev, SD3068_SECOND, buf, 7))
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

static struct device_t * rtc_sd3068_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct rtc_sd3068_pdata_t * pdat;
	struct rtc_t * rtc;
	struct device_t * dev;
	struct i2c_device_t * i2cdev;
	u8_t buf[8];

	i2cdev = i2c_device_alloc(dt_read_string(n, "i2c-bus", NULL), 0x32, 0);
	if(!i2cdev)
		return NULL;

	if(!sd3068_read(i2cdev, SD3068_ID, buf, 8))
	{
		i2c_device_free(i2cdev);
		return NULL;
	}

	pdat = malloc(sizeof(struct rtc_sd3068_pdata_t));
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
	rtc->settime = rtc_sd3068_settime;
	rtc->gettime = rtc_sd3068_gettime;
	rtc->priv = pdat;

	if(!(dev = register_rtc(rtc, drv)))
	{
		i2c_device_free(pdat->dev);
		free_device_name(rtc->name);
		free(rtc->priv);
		free(rtc);
		return NULL;
	}
	return dev;
}

static void rtc_sd3068_remove(struct device_t * dev)
{
	struct rtc_t * rtc = (struct rtc_t *)dev->priv;
	struct rtc_sd3068_pdata_t * pdat = (struct rtc_sd3068_pdata_t *)rtc->priv;

	if(rtc)
	{
		unregister_rtc(rtc);
		i2c_device_free(pdat->dev);
		free_device_name(rtc->name);
		free(rtc->priv);
		free(rtc);
	}
}

static void rtc_sd3068_suspend(struct device_t * dev)
{
}

static void rtc_sd3068_resume(struct device_t * dev)
{
}

static struct driver_t rtc_sd3068 = {
	.name		= "rtc-sd3068",
	.probe		= rtc_sd3068_probe,
	.remove		= rtc_sd3068_remove,
	.suspend	= rtc_sd3068_suspend,
	.resume		= rtc_sd3068_resume,
};

static __init void rtc_sd3068_driver_init(void)
{
	register_driver(&rtc_sd3068);
}

static __exit void rtc_sd3068_driver_exit(void)
{
	unregister_driver(&rtc_sd3068);
}

driver_initcall(rtc_sd3068_driver_init);
driver_exitcall(rtc_sd3068_driver_exit);
