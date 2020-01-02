/*
 * driver/rtc-rk818.c
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
	RK818_SECONDS		= 0x00,
	RK818_MINUTES		= 0x01,
	RK818_HOURS			= 0x02,
	RK818_DAYS			= 0x03,
	RK818_MONTHS		= 0x04,
	RK818_YEARS			= 0x05,
	RK818_WEEKS			= 0x06,
	RK818_ALARM_SECONDS	= 0x08,
	RK818_ALARM_MINUTES	= 0x09,
	RK818_ALARM_HOURS	= 0x0a,
	RK818_ALARM_DAYS	= 0x0b,
	RK818_ALARM_MONTHS	= 0x0c,
	RK818_ALARM_YEARS	= 0x0d,
	RK818_RTC_CTRL		= 0x10,
	RK818_RTC_STATUS	= 0x11,
	RK818_RTC_INT		= 0x12,
	RK818_RTC_COMP_LSB	= 0x13,
	RK818_RTC_COMP_MSB	= 0x14,
	RK818_CLK32OUT_REG	= 0x20,
};

struct rtc_rk818_pdata_t {
	struct i2c_device_t * dev;
};

static bool_t rk818_read(struct i2c_device_t * dev, u8_t reg, u8_t * val)
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

static bool_t rk818_write(struct i2c_device_t * dev, u8_t reg, u8_t val)
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

static bool_t rtc_rk818_settime(struct rtc_t * rtc, struct rtc_time_t * time)
{
	struct rtc_rk818_pdata_t * pdat = (struct rtc_rk818_pdata_t *)rtc->priv;
	u8_t val = 0;

	rk818_read(pdat->dev, RK818_RTC_CTRL, &val);
	val |= (0x1 << 0);
	rk818_write(pdat->dev, RK818_RTC_CTRL, val);

	rk818_write(pdat->dev, RK818_SECONDS, bin2bcd(time->second));
	rk818_write(pdat->dev, RK818_MINUTES, bin2bcd(time->minute));
	rk818_write(pdat->dev, RK818_HOURS, bin2bcd(time->hour));
	rk818_write(pdat->dev, RK818_DAYS, bin2bcd(time->day));
	rk818_write(pdat->dev, RK818_MONTHS, bin2bcd(time->month));
	rk818_write(pdat->dev, RK818_YEARS, bin2bcd(time->year - 2000));
	rk818_write(pdat->dev, RK818_WEEKS, bin2bcd(time->week));

	rk818_read(pdat->dev, RK818_RTC_CTRL, &val);
	val &= ~(0x1 << 0);
	rk818_write(pdat->dev, RK818_RTC_CTRL, val);

	return TRUE;
}

static bool_t rtc_rk818_gettime(struct rtc_t * rtc, struct rtc_time_t * time)
{
	struct rtc_rk818_pdata_t * pdat = (struct rtc_rk818_pdata_t *)rtc->priv;
	u8_t val = 0;

	rk818_read(pdat->dev, RK818_RTC_CTRL, &val);
	val &= ~(0x1 << 6);
	rk818_write(pdat->dev, RK818_RTC_CTRL, val);

	rk818_read(pdat->dev, RK818_RTC_CTRL, &val);
	val |= (0x1 << 6) | (0x1 << 7);
	rk818_write(pdat->dev, RK818_RTC_CTRL, val);

	rk818_read(pdat->dev, RK818_SECONDS, &val);
	time->second = bcd2bin(val & 0x7f);
	rk818_read(pdat->dev, RK818_MINUTES, &val);
	time->minute = bcd2bin(val & 0x7f);
	rk818_read(pdat->dev, RK818_HOURS, &val);
	time->hour = bcd2bin(val & 0x3f);
	rk818_read(pdat->dev, RK818_DAYS, &val);
	time->day = bcd2bin(val & 0x3f);
	rk818_read(pdat->dev, RK818_MONTHS, &val);
	time->month = bcd2bin(val & 0x1f);
	rk818_read(pdat->dev, RK818_YEARS, &val);
	time->year = bcd2bin(val & 0xff) + 2000;
	rk818_read(pdat->dev, RK818_WEEKS, &val);
	time->week = bcd2bin(val & 0x07);

	return TRUE;
}

static struct device_t * rtc_rk818_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct rtc_rk818_pdata_t * pdat;
	struct rtc_t * rtc;
	struct device_t * dev;
	struct i2c_device_t * i2cdev;
	u8_t val;

	i2cdev = i2c_device_alloc(dt_read_string(n, "i2c-bus", NULL), dt_read_int(n, "slave-address", 0x1c), 0);
	if(!i2cdev)
		return NULL;

	if(rk818_read(i2cdev, RK818_RTC_CTRL, &val))
	{
		if(val & (0x1 << 0))
		{
			rk818_write(i2cdev, RK818_SECONDS, bin2bcd(0));
			rk818_write(i2cdev, RK818_MINUTES, bin2bcd(0));
			rk818_write(i2cdev, RK818_HOURS, bin2bcd(0));
			rk818_write(i2cdev, RK818_DAYS, bin2bcd(1));
			rk818_write(i2cdev, RK818_MONTHS, bin2bcd(1));
			rk818_write(i2cdev, RK818_YEARS, bin2bcd(2016 - 2000));
			rk818_write(i2cdev, RK818_WEEKS, bin2bcd(6));

			val &= ~((0x1 << 0) | (0x1 << 3));
			rk818_write(i2cdev, RK818_RTC_CTRL, val);
		}

		rk818_read(i2cdev, RK818_CLK32OUT_REG, &val);
		val |= (0x1 << 0);
		rk818_write(i2cdev, RK818_CLK32OUT_REG, val);
	}
	else
	{
		i2c_device_free(i2cdev);
		return NULL;
	}

	pdat = malloc(sizeof(struct rtc_rk818_pdata_t));
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
	rtc->settime = rtc_rk818_settime;
	rtc->gettime = rtc_rk818_gettime;
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

static void rtc_rk818_remove(struct device_t * dev)
{
	struct rtc_t * rtc = (struct rtc_t *)dev->priv;
	struct rtc_rk818_pdata_t * pdat = (struct rtc_rk818_pdata_t *)rtc->priv;

	if(rtc)
	{
		unregister_rtc(rtc);
		i2c_device_free(pdat->dev);
		free_device_name(rtc->name);
		free(rtc->priv);
		free(rtc);
	}
}

static void rtc_rk818_suspend(struct device_t * dev)
{
}

static void rtc_rk818_resume(struct device_t * dev)
{
}

static struct driver_t rtc_rk818 = {
	.name		= "rtc-rk818",
	.probe		= rtc_rk818_probe,
	.remove		= rtc_rk818_remove,
	.suspend	= rtc_rk818_suspend,
	.resume		= rtc_rk818_resume,
};

static __init void rtc_rk818_driver_init(void)
{
	register_driver(&rtc_rk818);
}

static __exit void rtc_rk818_driver_exit(void)
{
	unregister_driver(&rtc_rk818);
}

driver_initcall(rtc_rk818_driver_init);
driver_exitcall(rtc_rk818_driver_exit);
