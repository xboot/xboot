/*
 * driver/i2c/i2c-algo-bit.c
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
#include <i2c/i2c-algo-bit.h>

static inline void sdalo(struct i2c_algo_bit_data_t * bdat)
{
	bdat->setsda(bdat, 0);
	udelay((bdat->udelay + 1) / 2);
}

static inline void sdahi(struct i2c_algo_bit_data_t * bdat)
{
	bdat->setsda(bdat, 1);
	udelay((bdat->udelay + 1) / 2);
}

static inline void scllo(struct i2c_algo_bit_data_t * bdat)
{
	bdat->setscl(bdat, 0);
	udelay(bdat->udelay / 2);
}

static int sclhi(struct i2c_algo_bit_data_t * bdat)
{
	bdat->setscl(bdat, 1);

	if(!bdat->getscl)
	{
		udelay(bdat->udelay);
		return 0;
	}

	ktime_t timeout = ktime_add_ms(ktime_get(), 100);
	while(!bdat->getscl(bdat))
	{
		if(ktime_after(ktime_get(), timeout))
		{
			if(bdat->getscl(bdat))
				break;
			return -1;
		}
	}

	udelay(bdat->udelay);
	return 0;
}

static void i2c_start(struct i2c_algo_bit_data_t * bdat)
{
	bdat->setsda(bdat, 0);
	udelay(bdat->udelay);
	scllo(bdat);
}

static void i2c_repstart(struct i2c_algo_bit_data_t * bdat)
{
	sdahi(bdat);
	sclhi(bdat);
	bdat->setsda(bdat, 0);
	udelay(bdat->udelay);
	scllo(bdat);
}

static void i2c_stop(struct i2c_algo_bit_data_t * bdat)
{
	sdalo(bdat);
	sclhi(bdat);
	bdat->setsda(bdat, 1);
	udelay(bdat->udelay);
}

static int i2c_outb(struct i2c_algo_bit_data_t * bdat, unsigned char c)
{
	int i;
	int sb;
	int ack;

	for(i = 7; i >= 0; i--)
	{
		sb = (c >> i) & 1;
		bdat->setsda(bdat, sb);
		udelay((bdat->udelay + 1) / 2);

		if(sclhi(bdat) < 0)
			return -1;
		scllo(bdat);
	}

	sdahi(bdat);

	if(sclhi(bdat) < 0)
		return -1;

	ack = !bdat->getsda(bdat);
	scllo(bdat);

	return ack;
}

static int i2c_inb(struct i2c_algo_bit_data_t * bdat)
{
	int i;
	unsigned char indata = 0;

	sdahi(bdat);

	for(i = 0; i < 8; i++)
	{
		if(sclhi(bdat) < 0)
			return -1;

		indata *= 2;
		if(bdat->getsda(bdat))
			indata |= 0x01;
		bdat->setscl(bdat, 0);
		udelay(i == 7 ? bdat->udelay / 2 : bdat->udelay);
	}

	return indata;
}

static int try_address(struct i2c_algo_bit_data_t * bdat, unsigned char addr, int retries)
{
	int i, ret = 0;

	for (i = 0; i <= retries; i++)
	{
		ret = i2c_outb(bdat, addr);
		if (ret == 1 || i == retries)
			break;
		i2c_stop(bdat);
		udelay(bdat->udelay);
		i2c_start(bdat);
	}

	return ret;
}

static int sendbytes(struct i2c_algo_bit_data_t * bdat, struct i2c_msg_t * msg)
{
	const unsigned char * temp = msg->buf;
	int count = msg->len;
	unsigned short nak_ok = msg->flags & I2C_M_IGNORE_NAK;
	int retval;
	int wrcount = 0;

	while(count > 0)
	{
		retval = i2c_outb(bdat, *temp);

		if((retval > 0) || (nak_ok && (retval == 0)))
		{
			count--;
			temp++;
			wrcount++;
		}
		else if (retval == 0)
		{
			return -1;
		}
		else
		{
			return retval;
		}
	}

	return wrcount;
}

static int acknak(struct i2c_algo_bit_data_t * bdat, int is_ack)
{
	if(is_ack)
		bdat->setsda(bdat, 0);
	udelay((bdat->udelay + 1) / 2);

	if(sclhi(bdat) < 0)
		return -1;

	scllo(bdat);
	return 0;
}

static int readbytes(struct i2c_algo_bit_data_t * bdat, struct i2c_msg_t * msg)
{
	int inval;
	int rdcount = 0;
	unsigned char *temp = msg->buf;
	int count = msg->len;
	const unsigned flags = msg->flags;

	while(count > 0)
	{
		inval = i2c_inb(bdat);
		if(inval >= 0)
		{
			*temp = inval;
			rdcount++;
		}
		else
		{
			break;
		}

		temp++;
		count--;

		if(rdcount == 1 && (flags & I2C_M_RECV_LEN))
		{
			if (inval <= 0 || inval > 32)
			{
				if(!(flags & I2C_M_NO_RD_ACK))
					acknak(bdat, 0);
				return -1;
			}

			count += inval;
			msg->len += inval;
		}

		if(!(flags & I2C_M_NO_RD_ACK))
		{
			inval = acknak(bdat, count);
			if(inval < 0)
				return inval;
		}
	}

	return rdcount;
}

static int bit_do_address(struct i2c_algo_bit_data_t * bdat, struct i2c_msg_t * msg)
{
	unsigned short flags = msg->flags;
	unsigned short nak_ok = msg->flags & I2C_M_IGNORE_NAK;
	unsigned char addr;
	int ret, retries;

	retries = nak_ok ? 0 : 3;

	if (flags & I2C_M_TEN)
	{
		addr = 0xf0 | ((msg->addr >> 7) & 0x06);

		ret = try_address(bdat, addr, retries);
		if((ret != 1) && !nak_ok)
		{
			return -1;
		}

		ret = i2c_outb(bdat, msg->addr & 0xff);
		if((ret != 1) && !nak_ok)
		{
			return -1;
		}

		if(flags & I2C_M_RD)
		{
			i2c_repstart(bdat);

			addr |= 0x01;
			ret = try_address(bdat, addr, retries);
			if((ret != 1) && !nak_ok)
			{
				return -1;
			}
		}
	}
	else
	{
		addr = msg->addr << 1;
		if(flags & I2C_M_RD)
			addr |= 1;
		if(flags & I2C_M_REV_DIR_ADDR)
			addr ^= 1;
		ret = try_address(bdat, addr, retries);
		if((ret != 1) && !nak_ok)
			return -1;
	}

	return 0;
}

int i2c_algo_bit_xfer(struct i2c_algo_bit_data_t * bdat, struct i2c_msg_t * msgs, int num)
{
	struct i2c_msg_t * pmsg;
	int i, ret;
	unsigned short nak_ok;

	i2c_start(bdat);
	for(i = 0; i < num; i++)
	{
		pmsg = &msgs[i];
		nak_ok = pmsg->flags & I2C_M_IGNORE_NAK;
		if(!(pmsg->flags & I2C_M_NOSTART))
		{
			if(i)
			{
				i2c_repstart(bdat);
			}
			ret = bit_do_address(bdat, pmsg);
			if((ret != 0) && !nak_ok)
			{
				goto bailout;
			}
		}
		if(pmsg->flags & I2C_M_RD)
		{
			ret = readbytes(bdat, pmsg);
			if(ret < pmsg->len)
			{
				if (ret >= 0)
					ret = -1;
				goto bailout;
			}
		}
		else
		{
			ret = sendbytes(bdat, pmsg);
			if(ret < pmsg->len)
			{
				if (ret >= 0)
					ret = -1;
				goto bailout;
			}
		}
	}
	ret = i;

bailout:
	i2c_stop(bdat);
	return ret;
}
