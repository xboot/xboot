/*
 * lib/libc/io.c
 */

#include <configs.h>
#include <default.h>
#include <types.h>
#include <byteorder.h>
#include <io.h>

/*
 * read 8-bit bytes from port into memory
 */
void readsb(const void * port, void * dst, x_s32 count)
{
	x_u8 * p = (x_u8 *)dst;
	x_u32 w;

	while(((x_u32)p) & 0x3)
	{
		if(count <= 0)
			return;
		count--;
		*p = readb((x_sys)port);
		p++;
	}

	while(count >= 4)
	{
		count -= 4;
		w = readb((x_sys)port) << 24;
		w |= readb((x_sys)port) << 16;
		w |= readb((x_sys)port) << 8;
		w |= readb((x_sys)port);
		*(x_u32 *) p = w;
		p += 4;
	}

	while(count > 0)
	{
		--count;
		*p = readb((x_sys)port);
		p++;
	}
}

/*
 * read 16-bit half words from port into memory
 */
void readsw(const void * port, void * dst, x_s32 count)
{
	x_u8 * p = (x_u8 *)dst;
	x_u32 l = 0, l2;

	if(count <= 0)
		return;

	switch(((x_u32)p) & 0x3)
	{
	/* buffer 32-bit aligned */
	case 0x00:
		while(count >= 2)
		{
			count -= 2;
			l = cpu_to_le16(readw((x_sys)port)) << 16;
			l |= cpu_to_le16(readw((x_sys)port));
			*(x_u32 *)p = l;
			p += 4;
		}
		if(count > 0)
		{
			*(x_u16 *)p = cpu_to_le16(readw((x_sys)port));
		}
		break;

	/* buffer 16-bit aligned */
	case 0x02:
		*(x_u16 *)p = cpu_to_le16(readw((x_sys)port));
		p += 2;
		count--;
		while(count>=2)
		{
			count -= 2;
			l = cpu_to_le16(readw((x_sys)port)) << 16;
			l |= cpu_to_le16(readw((x_sys)port));
			*(x_u16 *)p = l;
			p += 4;
		}
		if(count)
		{
			*(x_u16 *)p = cpu_to_le16(readw((x_sys)port));
		}
		break;

	/* buffer 8-bit aligned */
	case 0x01:
	case 0x03:
		--count;
		l = cpu_to_le16(readw((x_sys)port));
		*p = l >> 8;
		p++;
		while(count--)
		{
			l2 = cpu_to_le16(readw((x_sys)port));
			*(x_u16 *)p = (l & 0xff) << 8 | (l2 >> 8);
			p += 2;
			l = l2;
		}
		*p = l & 0xff;
		break;
	}
}

/*
 * read 32-bit words from port into memory
 */
void readsl(const void * port, void * dst, x_s32 count)
{
	x_u8 * p = (x_u8 *)dst;
	x_u32 l = 0, l2;

	if(count <= 0)
		return;

	switch(((x_u32) dst) & 0x3)
	{
	/* buffer 32-bit aligned */
	case 0x00:
		while(count--)
		{
			*(x_u32 *)p = cpu_to_le32(readl((x_sys)port));
			p += 4;
		}
		break;

	/* buffer 16-bit aligned */
	case 0x02:
		--count;
		l = cpu_to_le32(readl((x_sys)port));
		*(x_u16 *)p = l >> 16;
		p += 2;

		while(count--)
		{
			l2 = cpu_to_le32(readl((x_sys)port));
			*(x_u32 *)p = (l & 0xffff) << 16 | (l2 >> 16);
			p += 4;
			l = l2;
		}
		*(x_u16 *)p = l & 0xffff;
		break;

	/* buffer 8-bit aligned */
	case 0x01:
		--count;
		l = cpu_to_le32(readl((x_sys)port));
		*(x_u8 *)p = l >> 24;
		p++;
		*(x_u16 *)p = (l >> 8) & 0xffff;
		p += 2;
		while(count--)
		{
			l2 = cpu_to_le32(readl((x_sys)port));
			*(x_u32 *)p = (l & 0xff) << 24 | (l2 >> 8);
			p += 4;
			l = l2;
		}
		*p = l & 0xff;
		break;

	/* buffer 8-bit aligned */
	case 0x03:
		--count;
		l = cpu_to_le32(readl((x_sys)port));
		*p = l >> 24;
		p++;
		while(count--)
		{
			l2 = cpu_to_le32(readl((x_sys)port));
			*(x_u32 *)p = (l & 0xffffff) << 8 | l2 >> 24;
			p += 4;
			l = l2;
		}
		*(x_u16 *)p = (l >> 8) & 0xffff;
		p += 2;
		*p = l & 0xff;
		break;
	}
}
