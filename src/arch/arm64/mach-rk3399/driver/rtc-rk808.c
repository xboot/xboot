/*
 * driver/rtc-rk808.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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
	RK808_SECONDS		= 0x00,
	RK808_MINUTES		= 0x01,
	RK808_HOURS			= 0x02,
	RK808_DAYS			= 0x03,
	RK808_MONTHS		= 0x04,
	RK808_YEARS			= 0x05,
	RK808_WEEKS			= 0x06,
	RK808_ALARM_SECONDS	= 0x07,
	RK808_ALARM_MINUTES	= 0x08,
	RK808_ALARM_HOURS	= 0x09,
	RK808_ALARM_DAYS	= 0x0a,
	RK808_ALARM_MONTHS	= 0x0b,
	RK808_ALARM_YEARS	= 0x0c,
	RK808_RTC_CTRL		= 0x10,
	RK808_RTC_STATUS	= 0x11,
	RK808_RTC_INT		= 0x12,
	RK808_RTC_COMP_LSB	= 0x13,
	RK808_RTC_COMP_MSB	= 0x14,
	RK808_CLK32OUT_REG	= 0x20,
};

struct rtc_rk808_pdata_t {
	struct i2c_device_t * dev;
};

static bool_t rk808_read(struct i2c_device_t * dev, u8_t reg, u8_t * val)
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

static bool_t rk808_write(struct i2c_device_t * dev, u8_t reg, u8_t val)
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

static bool_t rtc_rk808_settime(struct rtc_t * rtc, struct rtc_time_t * time)
{
	struct rtc_rk808_pdata_t * pdat = (struct rtc_rk808_pdata_t *)rtc->priv;
	u8_t val = 0;

	rk808_read(pdat->dev, RK808_RTC_CTRL, &val);
	val |= (0x1 << 0);
	rk808_write(pdat->dev, RK808_RTC_CTRL, val);

	rk808_write(pdat->dev, RK808_SECONDS, bin2bcd(time->second));
	rk808_write(pdat->dev, RK808_MINUTES, bin2bcd(time->minute));
	rk808_write(pdat->dev, RK808_HOURS, bin2bcd(time->hour));
	rk808_write(pdat->dev, RK808_DAYS, bin2bcd(time->day));
	rk808_write(pdat->dev, RK808_MONTHS, bin2bcd(time->month));
	rk808_write(pdat->dev, RK808_YEARS, bin2bcd(time->year - 2000));
	rk808_write(pdat->dev, RK808_WEEKS, bin2bcd(time->week));

	rk808_read(pdat->dev, RK808_RTC_CTRL, &val);
	val &= ~(0x1 << 0);
	rk808_write(pdat->dev, RK808_RTC_CTRL, val);

	return TRUE;
}

static bool_t rtc_rk808_gettime(struct rtc_t * rtc, struct rtc_time_t * time)
{
	struct rtc_rk808_pdata_t * pdat = (struct rtc_rk808_pdata_t *)rtc->priv;
	u8_t val = 0;

	rk808_read(pdat->dev, RK808_RTC_CTRL, &val);
	val &= ~(0x1 << 6);
	rk808_write(pdat->dev, RK808_RTC_CTRL, val);

	rk808_read(pdat->dev, RK808_RTC_CTRL, &val);
	val |= (0x1 << 6) | (0x1 << 7);
	rk808_write(pdat->dev, RK808_RTC_CTRL, val);

	rk808_read(pdat->dev, RK808_SECONDS, &val);
	time->second = bcd2bin(val & 0x7f);
	rk808_read(pdat->dev, RK808_MINUTES, &val);
	time->minute = bcd2bin(val & 0x7f);
	rk808_read(pdat->dev, RK808_HOURS, &val);
	time->hour = bcd2bin(val & 0x3f);
	rk808_read(pdat->dev, RK808_DAYS, &val);
	time->day = bcd2bin(val & 0x3f);
	rk808_read(pdat->dev, RK808_MONTHS, &val);
	time->month = bcd2bin(val & 0x1f);
	rk808_read(pdat->dev, RK808_YEARS, &val);
	time->year = bcd2bin(val & 0xff) + 2000;
	rk808_read(pdat->dev, RK808_WEEKS, &val);
	time->week = bcd2bin(val & 0x07);

	return TRUE;
}

static struct device_t * rtc_rk808_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct rtc_rk808_pdata_t * pdat;
	struct rtc_t * rtc;
	struct device_t * dev;
	struct i2c_device_t * i2cdev;
	u8_t val;

	i2cdev = i2c_device_alloc(dt_read_string(n, "i2c-bus", NULL), dt_read_int(n, "slave-address", 0x1b), 0);
	if(!i2cdev)
		return NULL;

	if(rk808_read(i2cdev, RK808_RTC_CTRL, &val))
	{
		if(val & (0x1 << 0))
		{
			rk808_write(i2cdev, RK808_SECONDS, bin2bcd(0));
			rk808_write(i2cdev, RK808_MINUTES, bin2bcd(0));
			rk808_write(i2cdev, RK808_HOURS, bin2bcd(0));
			rk808_write(i2cdev, RK808_DAYS, bin2bcd(1));
			rk808_write(i2cdev, RK808_MONTHS, bin2bcd(1));
			rk808_write(i2cdev, RK808_YEARS, bin2bcd(2016 - 2000));
			rk808_write(i2cdev, RK808_WEEKS, bin2bcd(6));

			val &= ~((0x1 << 0) | (0x1 << 3));
			rk808_write(i2cdev, RK808_RTC_CTRL, val);
		}

		rk808_read(i2cdev, RK808_CLK32OUT_REG, &val);
		val |= (0x1 << 0);
		rk808_write(i2cdev, RK808_CLK32OUT_REG, val);
	}
	else
	{
		i2c_device_free(i2cdev);
		return NULL;
	}

	pdat = malloc(sizeof(struct rtc_rk808_pdata_t));
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
	rtc->settime = rtc_rk808_settime;
	rtc->gettime = rtc_rk808_gettime;
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

static void rtc_rk808_remove(struct device_t * dev)
{
	struct rtc_t * rtc = (struct rtc_t *)dev->priv;
	struct rtc_rk808_pdata_t * pdat = (struct rtc_rk808_pdata_t *)rtc->priv;

	if(rtc)
	{
		unregister_rtc(rtc);
		i2c_device_free(pdat->dev);
		free_device_name(rtc->name);
		free(rtc->priv);
		free(rtc);
	}
}

static void rtc_rk808_suspend(struct device_t * dev)
{
}

static void rtc_rk808_resume(struct device_t * dev)
{
}

static struct driver_t rtc_rk808 = {
	.name		= "rtc-rk808",
	.probe		= rtc_rk808_probe,
	.remove		= rtc_rk808_remove,
	.suspend	= rtc_rk808_suspend,
	.resume		= rtc_rk808_resume,
};

static __init void rtc_rk808_driver_init(void)
{
	register_driver(&rtc_rk808);
}

static __exit void rtc_rk808_driver_exit(void)
{
	unregister_driver(&rtc_rk808);
}

driver_initcall(rtc_rk808_driver_init);
driver_exitcall(rtc_rk808_driver_exit);
