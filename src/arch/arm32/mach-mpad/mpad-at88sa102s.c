/*
 * mpad-at88sa102s.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
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
#include <s5pv210/reg-gpio.h>
#include <s5pv210-cp15.h>
#include <mpad-at88sa102s.h>

#define START_PULSE_TIME_OUT	(500)		/* 86us * 2 * 10 */

static void sa_delay_us(u32_t loop)
{
	u32_t i = 0;

	while(loop--)
	{
		i++;
		i++;
		i++;
	}
}

static void sa_delay_4340_ns(void)
{
	u32_t i = 4;

	while(--i)
	{
		__asm__ __volatile__("mov r0, r0");
	}
}

static void sa_delay_9000_ns(void)
{
	u32_t i = 17;

	while(--i)
	{
		__asm__ __volatile__("mov r0, r0");
	}
}


static void sa_enable_interrupt(void)
{
	irq_enable();
	fiq_enable();
}

static void sa_disable_interrupt(void)
{
	irq_disable();
	fiq_disable();
}

static void sa_pin_strength(void)
{
	writel(S5PV210_GPH1DRV, (readl(S5PV210_GPH1DRV) & ~(0xf<<2)) | (0x3<<2));
}

static void sa_pin_dir(u8_t flag)
{
	if(flag)	/* output */
		writel(S5PV210_GPH1CON, (readl(S5PV210_GPH1CON) & ~(0xf<<4)) | (0x1<<4));
	else		/* input */
		writel(S5PV210_GPH1CON, (readl(S5PV210_GPH1CON) & ~(0xf<<4)) | (0x0<<4));
}

static void sa_pin_set(u8_t flag)
{
	if(flag)	/* high level */
		writel(S5PV210_GPH1DAT, (readl(S5PV210_GPH1DAT) & ~(0x1<<1)) | (0x1<<1));
	else		/* low level */
		writel(S5PV210_GPH1DAT, (readl(S5PV210_GPH1DAT) & ~(0x1<<1)) | (0x0<<1));
}

static u8_t sa_pin_get(void)
{
	if(readl(S5PV210_GPH1DAT) & (0x1<<1))
		return 1;
	else
		return 0;
}


#define IO_FLAGS_COMMAND		(0x77)
#define IO_FLAGS_TRANSMIT		(0x88)
#define IO_FLAGS_SLEEP			(0xcc)

#define OPCODE_MAC				(0x08)
#define OPCODE_READ				(0x02)
#define OPCODE_BURN_FUSE		(0x04)
#define OPCODE_GEN_PERS			(0x20)
#define OPCODE_BURN_SECURE		(0x10)
#define OPCODE_PAUSE_LONG		(0x01)

#define DELAY_SLEEP				(10000)
#define DELAY_EXEC_READ			(3000)
#define DELAY_EXEC_MAC			(30000)
#define DELAY_EXEC_FUSE			(190000)
#define DELAY_EXEC_SECURE		(190000)

struct sa_buffer {
	u8_t data[128];
	u8_t count;
	u16_t crc;
};

static u16_t sa_crc16(u8_t * buf, u8_t len)
{
	u16_t crc = 0x0000;
	u16_t i;
	u8_t cnt;
	u8_t bit, cbit;

	for (cnt = 0; cnt < len; cnt++)
	{
		for (i = 0x1; i < 0x100; i = i << 1)
		{
			if (buf[cnt] & (u8_t) i)
			{
				bit = 0x01;
			}
			else
			{
				bit = 0x00;
			}

			if (0x8000 & crc)
			{
				cbit = 0x01;
			}
			else
			{
				cbit = 0x00;
			}

			if (bit ^ cbit)
			{
				crc = ((crc << 1) & 0xffff) ^ 0x8005;
			}
			else
				crc = (crc << 1) & 0xFFFF;
		}
	}

	return crc;
}

static void sa_wakeup(void)
{
	/*
	 * port init
	 */
	sa_pin_set(0);
	sa_pin_dir(0);
	sa_pin_strength();
	sa_delay_us(5000);

	/*
	 * wake low
	 */
	sa_pin_dir(1);
	sa_delay_us(80);

	/*
	 * wake high
	 */
	sa_pin_dir(0);
	sa_delay_us(4000);
}

static void sa_write_one(void)
{
	/*
	 * start bit
	 */
	sa_pin_dir(1);
	sa_delay_4340_ns();

	/*
	 * 7-bits delay
	 */
	sa_pin_dir(0);
	sa_delay_4340_ns();

	sa_pin_dir(0);
	sa_delay_4340_ns();

	sa_pin_dir(0);
	sa_delay_4340_ns();

	sa_pin_dir(0);
	sa_delay_4340_ns();

	sa_pin_dir(0);
	sa_delay_4340_ns();

	sa_pin_dir(0);
	sa_delay_4340_ns();

	sa_pin_dir(0);
	sa_delay_4340_ns();

	sa_pin_dir(0);
	sa_delay_4340_ns();
}

static void sa_write_zero(void)
{
	/*
	 * start bit
	 */
	sa_pin_dir(1);
	sa_delay_4340_ns();

	/*
	 * send high
	 */
	sa_pin_dir(0);
	sa_delay_4340_ns();

	/*
	 * send low
	 */
	sa_pin_dir(1);
	sa_delay_4340_ns();

	/*
	 * 5-bits delay
	 */
	sa_pin_dir(0);
	sa_delay_4340_ns();

	sa_pin_dir(0);
	sa_delay_4340_ns();

	sa_pin_dir(0);
	sa_delay_4340_ns();

	sa_pin_dir(0);
	sa_delay_4340_ns();

	sa_pin_dir(0);
	sa_delay_4340_ns();

	sa_pin_dir(0);
	sa_delay_4340_ns();
}

static void sa_write_byte(u8_t c)
{
	u8_t i;

	for(i = 0; i < 8; i++)
	{
		if(c & (0x1 << i))
			sa_write_one();
		else
			sa_write_zero();
	}
}

static bool_t sa_read_bit(u8_t * bit)
{
	u32_t time_out;

	/*
	 * wait for start pluse
	 */
	time_out = START_PULSE_TIME_OUT;

	/*
	 * falling edge
	 */
	while(--time_out)
	{
		if(!sa_pin_get())
			break;
	}

	if(time_out == 0)
		return FALSE;

	/*
	 * wait for start pluse
	 */
	time_out = START_PULSE_TIME_OUT;

	/*
	 * rising edge
	 */
	do {
		if(sa_pin_get())
			break;
	} while (--time_out);

	if(time_out == 0)
		return FALSE;

	/*
	 * detect possible edge indicating zero bit.
	 */
	sa_delay_9000_ns();

	if(!sa_pin_get())
		*bit = 0x0;
	else
		*bit = 0x1;

	sa_delay_9000_ns();

	return TRUE;
}

static bool_t sa_read_byte(u8_t * c)
{
	u8_t byte = 0;
	u8_t bit;
	u8_t i;

	for(i = 0; i < 8; i++)
	{
		if(!sa_read_bit(&bit))
			return FALSE;

		byte |= (bit << i);
	}

	*c = byte;
	return TRUE;
}

static void sa_sleep(void)
{
	sa_write_byte(IO_FLAGS_SLEEP);
	sa_delay_us(DELAY_SLEEP);
}

static inline bool_t sa_command(struct sa_buffer * cmd, u32_t delay)
{
	u8_t i;

	cmd->count = cmd->data[0];
	cmd->crc = sa_crc16(&(cmd->data[0]), (cmd->data[0] - 2));
	cmd->data[cmd->count - 2] = cmd->crc & 0xff;
	cmd->data[cmd->count - 1] = (cmd->crc >> 8) & 0xff;

	sa_write_byte(IO_FLAGS_COMMAND);

	for (i = 0; i < cmd->count; i++)
	{
		sa_write_byte(cmd->data[i]);
	}

	sa_delay_us(delay);
	return TRUE;
}

static inline bool_t sa_transmit(struct sa_buffer * rep)
{
	u8_t i;

	sa_write_byte(IO_FLAGS_TRANSMIT);

	if(!sa_read_byte(&(rep->data[0])))
	{
		sa_sleep();
		return FALSE;
	}

	for (i = 1; i < rep->data[0]; i++)
	{
		if(!sa_read_byte(&(rep->data[i])))
		{
			sa_sleep();
			return FALSE;
		}
	}

	rep->count = rep->data[0];
	rep->crc = sa_crc16(&(rep->data[0]), (rep->data[0] - 2));

	if( ((rep->crc & 0xff) != rep->data[rep->count - 2]) || (((rep->crc >> 8) & 0xff) != rep->data[rep->count - 1]) )
	{
		sa_sleep();
		return FALSE;
	}

	return TRUE;
}

static bool_t sa_process(struct sa_buffer * cmd, struct sa_buffer * rep, u32_t delay)
{
	struct sa_buffer buf;

	if(!cmd || !rep)
		return FALSE;

	/*
	 * disable interrupt
	 */
	sa_disable_interrupt();

	/*
	 * Chip wakeup
	 */
	sa_wakeup();
	if(!sa_transmit(&buf))
	{
		sa_enable_interrupt();
		return FALSE;
	}

	/*
	 * Send command
	 */
	if(!sa_command(cmd, delay))
	{
		sa_enable_interrupt();
		return FALSE;
	}

	/*
	 * get response
	 */
	if(!sa_transmit(rep))
	{
		sa_enable_interrupt();
		return FALSE;
	}

	/*
	 * Chip sleep
	 */
	sa_sleep();

	/*
	 * enable interrupt
	 */
	sa_enable_interrupt();
	return TRUE;
}

static u8_t sa_read_rom(u32_t addr, u8_t * buf)
{
	struct sa_buffer cmd;
	struct sa_buffer rep;

	cmd.data[0] = 7;
	cmd.data[1] = OPCODE_READ;
	cmd.data[2] = 0x0;
	cmd.data[3] = addr & 0xff;
	cmd.data[4] = (addr >> 8) & 0xff;

	if(!sa_process(&cmd, &rep, DELAY_EXEC_READ))
		return 0;

	if(rep.count < 3)
		return 0;

	memcpy(buf, &rep.data[1], (rep.count - 3));
	return (rep.count - 3);
}

static u8_t sa_read_fuse(u32_t addr, u8_t * buf)
{
	struct sa_buffer cmd;
	struct sa_buffer rep;

	cmd.data[0] = 7;
	cmd.data[1] = OPCODE_READ;
	cmd.data[2] = 0x1;
	cmd.data[3] = addr & 0xff;
	cmd.data[4] = (addr >> 8) & 0xff;

	if(!sa_process(&cmd, &rep, DELAY_EXEC_READ))
		return 0;

	if(rep.count < 3)
		return 0;

	memcpy(buf, &rep.data[1], (rep.count - 3));
	return (rep.count - 3);
}

static u8_t sa_read_mac(u8_t mode, u16_t keyid, const u8_t * challenge, u8_t * mac)
{
	struct sa_buffer cmd;
	struct sa_buffer rep;

	cmd.data[0] = 0x27;
	cmd.data[1] = OPCODE_MAC;
	cmd.data[2] = mode;
	cmd.data[3] = keyid & 0xff;
	cmd.data[4] = (keyid >> 8) & 0xff;
	memcpy(&cmd.data[5], challenge, 32);

	if(!sa_process(&cmd, &rep, DELAY_EXEC_MAC))
		return 0;

	if(rep.count < 3)
		return 0;

	memcpy(mac, &rep.data[1], (rep.count - 3));
	return (rep.count - 3);
}

static bool_t sa_burn_fuse_zero(u8_t fuse_num)
{
	struct sa_buffer cmd;
	struct sa_buffer rep;

	cmd.data[0] = 0x07;
	cmd.data[1] = OPCODE_BURN_FUSE;
	cmd.data[2] = fuse_num;
	cmd.data[3] = 0x00;
	cmd.data[4] = 0x80;

	if(!sa_process(&cmd, &rep, DELAY_EXEC_FUSE))
		return FALSE;

	if(rep.count < 3)
		return FALSE;

	if(rep.data[1] != 0x00)
		return FALSE;

	return TRUE;
}

/*
 * burn 3-bytes fuse area
 */
bool_t sa_burn_fuse(u32_t fuse)
{
	u8_t i;

	fuse &= 0x00ffffff;

	for(i = 0; i < 24; i++)
	{
		if(! ((fuse >> i) & 0x1))
		{
			if(! sa_burn_fuse_zero(64 + i))
				return FALSE;
		}
	}

	return TRUE;
}

/*
 * Serial number is 48 bits / 6 bytes (16 bits from ROM, 32 bits from fuses).
 * It consists of bytes 2 and 3 of ROM address 0 combined with fuse[96 to 127].
 */
bool_t sa_read_serial_number(u8_t * buf)
{
	u8_t rom[32];
	u8_t fuse[32];
	u8_t len;

	len = sa_read_rom(0, rom);
	if(len != 4)
		return FALSE;

	len = sa_read_fuse(3, fuse);
	if(len != 4)
		return FALSE;

	buf[0] = rom[2];
	buf[1] = rom[3];

	buf[2] = fuse[0];
	buf[3] = fuse[1];
	buf[4] = fuse[2];
	buf[5] = fuse[3];

	return TRUE;
}

/*
 * This function reads the three-byte manufacturer id.
 *
 * ManufacturerId is 24 bits / 3 bytes (16 bits from ROM, 8 bits from fuses).
 * It consists of bytes 0 and 1 of ROM address 0 combined with fuse[88 to 95].
 */
bool_t sa_read_manufacture_id(u8_t * buf)
{
	u8_t rom[32];
	u8_t fuse[32];
	u8_t len;

	len = sa_read_rom(0, rom);
	if(len != 4)
		return FALSE;

	len = sa_read_fuse(2, fuse);
	if(len != 4)
		return FALSE;

	buf[0] = rom[0];
	buf[1] = rom[1];
	buf[2] = fuse[3];

	return TRUE;
}

/*
 * This function reads the device revision, 4-bytes.
 */
bool_t sa_read_revision(u8_t * buf)
{
	u8_t rom[32];
	u8_t len;

	len = sa_read_rom(1, rom);
	if(len != 4)
		return FALSE;

	buf[0] = rom[0];
	buf[1] = rom[1];
	buf[2] = rom[2];
	buf[3] = rom[3];

	return TRUE;
}

/*
 *  This function reads status fuses, 3-bytes.
 */
bool_t sa_read_fuse_status(u8_t * buf)
{
	u8_t fuse[32];
	u8_t len;

	len = sa_read_fuse(2, fuse);
	if(len != 4)
		return FALSE;

	buf[0] = fuse[0];
	buf[1] = fuse[1];
	buf[2] = fuse[2];

	return TRUE;
}

/*
 * key: 8-bytes, when bit-1 is zero for burn secure lock.
 * fuse: 3-bytes, when bit-23 is zero for enable key secure area.
 */
bool_t sa_burn_secure(u8_t * key, u8_t * fuse)
{
	struct sa_buffer cmd;
	struct sa_buffer rep;

	cmd.data[0] = 0x12;
	cmd.data[1] = OPCODE_BURN_SECURE;
	cmd.data[2] = 0x00;
	cmd.data[3] = 0x00;
	cmd.data[4] = 0x80;

	cmd.data[5] = ~key[0];
	cmd.data[6] = ~key[1];
	cmd.data[7] = ~key[2];
	cmd.data[8] = ~key[3];
	cmd.data[9] = ~key[4];
	cmd.data[10] = ~key[5];
	cmd.data[11] = ~key[6];
	cmd.data[12] = ~key[7];

	cmd.data[13] = ~fuse[0];
	cmd.data[14] = ~fuse[1];
	cmd.data[15] = ~fuse[2];

	if(!sa_process(&cmd, &rep, DELAY_EXEC_SECURE))
		return FALSE;

	if(rep.count < 3)
		return FALSE;

	if(rep.data[1] != 0x00)
		return FALSE;

	return TRUE;
}

bool_t sa_do_mac(u8_t mode, u16_t keyid, const u8_t * challenge, const u8_t * expected)
{
	u8_t mac[32];
	u8_t len;
	u8_t i;

	if(!challenge || !expected)
		return FALSE;

	len = sa_read_mac(mode, keyid, challenge, mac);
	if(len != 32)
		return FALSE;

#if 0
	/*
	 * Print mac result from chip
	 */
	printk("mac: ");
	for(i = 0; i < len; i++)
	{
		if((i % 8) == 0)
			printk("\r\n");
		printk("0x%02x, ", mac[i]);
	}
	printk("\r\n");
#endif

	for(i = 0; i < len; i++)
	{
		if(mac[i] != expected[i])
			return FALSE;
	}

	return TRUE;
}

static const u8_t challenge[4][32] = {
	[0] = {
		0x72, 0x29, 0x4c, 0xb2, 0xb7, 0x60, 0xf8, 0x11,
		0xc8, 0xbb, 0x9b, 0x60, 0x45, 0xa4, 0x2e, 0x51,
		0x2a, 0x44, 0x4f, 0x7d, 0xdb, 0x63, 0x8a, 0x72,
		0x9d, 0x0e, 0x76, 0x76, 0x6c, 0x9a, 0x37, 0x3f,
	},

	[1] = {
		0xb2, 0xb2, 0x2a, 0x23, 0x5c, 0x1a, 0xab, 0xc3,
		0x3f, 0x1f, 0xe1, 0xf8, 0xf4, 0xa3, 0x37, 0xec,
		0x13, 0xc6, 0x44, 0x38, 0x8b, 0x00, 0xd8, 0x60,
		0xd1, 0x13, 0xec, 0xc0, 0x6b, 0x33, 0x21, 0xc5,
	},

	[2] = {
		0x5c, 0x13, 0xd3, 0xd4, 0x9a, 0x80, 0x07, 0xe5,
		0xa6, 0x40, 0x8f, 0xc6, 0x04, 0x2d, 0x6d, 0xa0,
		0xbc, 0xb2, 0xc6, 0x50, 0x38, 0x29, 0x74, 0xdf,
		0xac, 0xd5, 0x76, 0xe2, 0xbd, 0xd6, 0xac, 0x78,
	},

	[3] = {
		0x4e, 0x45, 0x53, 0x44, 0xc6, 0x6e, 0x11, 0x85,
		0x6c, 0x4a, 0xed, 0xb7, 0xcb, 0x96, 0xaa, 0xa5,
		0x2f, 0x8e, 0x74, 0x08, 0xd5, 0x63, 0xfe, 0xd5,
		0xcd, 0x32, 0x83, 0xb2, 0x37, 0xe4, 0x6b, 0x3f,
	},
};

static const u8_t expected[4][32] = {
	[0] = {
		0xfc, 0xdd, 0xe3, 0x3f, 0xc7, 0x56, 0x7b, 0xfc,
		0x7d, 0x42, 0xe0, 0x7c, 0x20, 0x0a, 0x8b, 0x0f,
		0xbf, 0xf5, 0xfa, 0x2a, 0x2e, 0xf5, 0x0b, 0x5b,
		0xf7, 0xa1, 0x4d, 0x3a, 0x68, 0x1f, 0x3a, 0xa3,
	},

	[1] = {
		0x0e, 0xf0, 0xd1, 0x80, 0x5b, 0xf7, 0x64, 0x14,
		0xb6, 0xa7, 0xd6, 0x7c, 0x01, 0xb3, 0x04, 0x9f,
		0xc2, 0xff, 0x06, 0x4c, 0x41, 0xc9, 0x00, 0x51,
		0x19, 0xb5, 0x8c, 0x86, 0xe6, 0x0f, 0xf8, 0x42,
	},

	[2] = {
		0x40, 0x31, 0xa2, 0x87, 0x01, 0xf2, 0x0b, 0x87,
		0x5b, 0xac, 0x3a, 0x31, 0xe7, 0xe2, 0x35, 0x5e,
		0xe7, 0x53, 0x78, 0x74, 0xfd, 0x95, 0x3a, 0x4d,
		0xc6, 0xbf, 0xb2, 0x17, 0xa7, 0x03, 0x6b, 0xda,
	},

	[3] = {
		0x0b, 0x5c, 0xdb, 0x5a, 0x63, 0xe5, 0x53, 0xc3,
		0xfc, 0x83, 0x4a, 0x0a, 0x4f, 0x32, 0xcb, 0x9d,
		0xb4, 0x35, 0xd7, 0x60, 0x13, 0x47, 0x33, 0x90,
		0xba, 0x4c, 0xb0, 0x2b, 0xbc, 0x8e, 0x14, 0x8d,
	},
};

bool_t sa_do_auth(void)
{
	u32_t index;
	u32_t count = 10;

	do {
		srand(jiffies + rand());
		index = rand() % 4;
		index = rand() % 4;

		if(sa_do_mac(0x10, 0xffff, challenge[index], expected[index]))
			return TRUE;
	} while(--count > 0);

	return FALSE;
}
