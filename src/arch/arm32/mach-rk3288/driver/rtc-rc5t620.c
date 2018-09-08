/*
 * driver/rtc-rc5t620.c
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

enum {
	RC5T620_LSIVER		= 0x00,
	RC5T620_OTPVER		= 0x01,
	RC5T620_IODAC		= 0x02,
	RC5T620_VINDAC		= 0x03,
	RC5T620_OUT32KEN	= 0x05,

	RC5T620_SEC			= 0xa0,
	RC5T620_MIN			= 0xa1,
	RC5T620_HOUR		= 0xa2,
	RC5T620_WEEK		= 0xa3,
	RC5T620_DAY			= 0xa4,
	RC5T620_MONTH		= 0xa5,
	RC5T620_YEAR		= 0xa6,
	RC5T620_RTCADJ		= 0xa7,
	RC5T620_DAL_SEC		= 0xa8,
	RC5T620_DAL_MIN		= 0xa9,
	RC5T620_DAL_HOUR	= 0xaa,
	RC5T620_DAL_DAY		= 0xab,
	RC5T620_DAL_MONTH	= 0xac,
	RC5T620_DAL_YEAR	= 0xad,
	RC5T620_RTCCNT1		= 0xae,
	RC5T620_RTCCNT2		= 0xaf,
};

struct rtc_rc5t620_pdata_t {
	struct i2c_device_t * dev;
};

static bool_t rc5t620_read(struct i2c_device_t * dev, u8_t reg, u8_t * val)
{
	struct i2c_msg_t msgs[2];
	u8_t buf;

	msgs[0].addr = dev->addr;
	msgs[0].flags = 0;
	msgs[0].len = 1;
	msgs[0].buf = &reg;

	msgs[1].addr = dev->addr;
	msgs[1].flags = I2C_M_RD;
	msgs[1].len = 1;
	msgs[1].buf = &buf;

	if(i2c_transfer(dev->i2c, msgs, 2) != 2)
		return FALSE;

	if(val)
		*val = buf;
	return TRUE;
}

static bool_t rc5t620_write(struct i2c_device_t * dev, u8_t reg, u8_t val)
{
	struct i2c_msg_t msg;
	u8_t buf[2];

	buf[0] = reg;
	buf[1] = val;
    msg.addr = dev->addr;
    msg.flags = 0;
    msg.len = 2;
    msg.buf = &buf[0];

    if(i2c_transfer(dev->i2c, &msg, 1) != 1)
    	return FALSE;
    return TRUE;
}

static bool_t rtc_rc5t620_settime(struct rtc_t * rtc, struct rtc_time_t * time)
{
	struct rtc_rc5t620_pdata_t * pdat = (struct rtc_rc5t620_pdata_t *)rtc->priv;

	rc5t620_write(pdat->dev, RC5T620_SEC, bin2bcd(time->second) & 0x7f);
	rc5t620_write(pdat->dev, RC5T620_MIN, bin2bcd(time->minute) & 0x7f);
	rc5t620_write(pdat->dev, RC5T620_HOUR, bin2bcd(time->hour) & 0x3f);
	rc5t620_write(pdat->dev, RC5T620_WEEK, bin2bcd(time->week) & 0x7);
	rc5t620_write(pdat->dev, RC5T620_DAY, bin2bcd(time->day) & 0x3f);
	rc5t620_write(pdat->dev, RC5T620_MONTH, bin2bcd(time->month) & 0x1f);
	rc5t620_write(pdat->dev, RC5T620_YEAR, bin2bcd(time->year - 2000) & 0xff);
	return TRUE;
}

static bool_t rtc_rc5t620_gettime(struct rtc_t * rtc, struct rtc_time_t * time)
{
	struct rtc_rc5t620_pdata_t * pdat = (struct rtc_rc5t620_pdata_t *)rtc->priv;
	u8_t val = 0;

	rc5t620_read(pdat->dev, RC5T620_SEC, &val);
	time->second = bcd2bin(val & 0x7f);
	rc5t620_read(pdat->dev, RC5T620_MIN, &val);
	time->minute = bcd2bin(val & 0x7f);
	rc5t620_read(pdat->dev, RC5T620_HOUR, &val);
	time->hour = bcd2bin(val & 0x3f);
	rc5t620_read(pdat->dev, RC5T620_WEEK, &val);
	time->week = bcd2bin(val & 0x07);
	rc5t620_read(pdat->dev, RC5T620_DAY, &val);
	time->day = bcd2bin(val & 0x3f);
	rc5t620_read(pdat->dev, RC5T620_MONTH, &val);
	time->month = bcd2bin(val & 0x1f);
	rc5t620_read(pdat->dev, RC5T620_YEAR, &val);
	time->year = bcd2bin(val & 0xff) + 2000;
	return TRUE;
}

static struct device_t * rtc_rc5t620_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct rtc_rc5t620_pdata_t * pdat;
	struct rtc_t * rtc;
	struct device_t * dev;
	struct i2c_device_t * i2cdev;
	u8_t val;

	i2cdev = i2c_device_alloc(dt_read_string(n, "i2c-bus", NULL), dt_read_int(n, "slave-address", 0x32), 0);
	if(!i2cdev)
		return NULL;

	if(rc5t620_read(i2cdev, RC5T620_LSIVER, &val) && (val == 0x03))
	{
		rc5t620_write(i2cdev, RC5T620_RTCCNT1, 0x20);
		rc5t620_read(i2cdev, RC5T620_RTCCNT2, &val);
		if(val & (0x1 << 4))
		{
			val &= ~(0x1 << 5 | 0x1 << 4);
			val |= (0x1 << 5 | 0x0 << 4);

			rc5t620_write(i2cdev, RC5T620_SEC, bin2bcd(0) & 0x7f);
			rc5t620_write(i2cdev, RC5T620_MIN, bin2bcd(0) & 0x7f);
			rc5t620_write(i2cdev, RC5T620_HOUR, bin2bcd(0) & 0x3f);
			rc5t620_write(i2cdev, RC5T620_WEEK, bin2bcd(6) & 0x7);
			rc5t620_write(i2cdev, RC5T620_DAY, bin2bcd(1) & 0x3f);
			rc5t620_write(i2cdev, RC5T620_MONTH, bin2bcd(1) & 0x1f);
			rc5t620_write(i2cdev, RC5T620_YEAR, bin2bcd(2016 - 2000) & 0xff);
		}
		val &= ~(0x1 << 8 | 0x1 << 2 | 0x1 << 0);
		rc5t620_write(i2cdev, RC5T620_RTCCNT2, val);
	}
	else
	{
		i2c_device_free(i2cdev);
		return NULL;
	}

	pdat = malloc(sizeof(struct rtc_rc5t620_pdata_t));
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
	rtc->settime = rtc_rc5t620_settime;
	rtc->gettime = rtc_rc5t620_gettime;
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

static void rtc_rc5t620_remove(struct device_t * dev)
{
	struct rtc_t * rtc = (struct rtc_t *)dev->priv;
	struct rtc_rc5t620_pdata_t * pdat = (struct rtc_rc5t620_pdata_t *)rtc->priv;

	if(rtc && unregister_rtc(rtc))
	{
		i2c_device_free(pdat->dev);

		free_device_name(rtc->name);
		free(rtc->priv);
		free(rtc);
	}
}

static void rtc_rc5t620_suspend(struct device_t * dev)
{
}

static void rtc_rc5t620_resume(struct device_t * dev)
{
}

static struct driver_t rtc_rc5t620 = {
	.name		= "rtc-rc5t620",
	.probe		= rtc_rc5t620_probe,
	.remove		= rtc_rc5t620_remove,
	.suspend	= rtc_rc5t620_suspend,
	.resume		= rtc_rc5t620_resume,
};

static __init void rtc_rc5t620_driver_init(void)
{
	register_driver(&rtc_rc5t620);
}

static __exit void rtc_rc5t620_driver_exit(void)
{
	unregister_driver(&rtc_rc5t620);
}

driver_initcall(rtc_rc5t620_driver_init);
driver_exitcall(rtc_rc5t620_driver_exit);
