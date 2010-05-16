/*
 * lib/libc/io.c
 */

#include <configs.h>
#include <default.h>
#include <types.h>
#include <byteorder.h>
#include <io.h>

/*
 * read 8-bit bytes from io to memory
 */
void readsb(const void * io, void * dst, x_s32 count)
{
	x_u8 * p = (x_u8 *)dst;
	x_u32 w;

	while(((x_u32)p) & 0x3)
	{
		if(count <= 0)
			return;
		count--;
		*p = readb((x_sys)io);
		p++;
	}

	while(count >= 4)
	{
		count -= 4;
		w = readb((x_sys)io) << 24;
		w |= readb((x_sys)io) << 16;
		w |= readb((x_sys)io) << 8;
		w |= readb((x_sys)io);
		*(x_u32 *) p = w;
		p += 4;
	}

	while(count > 0)
	{
		--count;
		*p = readb((x_sys)io);
		p++;
	}
}

/*
 * read 16-bit half words from io to memory
 */
void readsw(const void * io, void * dst, x_s32 count)
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
			l = cpu_to_le16(readw((x_sys)io)) << 16;
			l |= cpu_to_le16(readw((x_sys)io));
			*(x_u32 *)p = l;
			p += 4;
		}
		if(count > 0)
		{
			*(x_u16 *)p = cpu_to_le16(readw((x_sys)io));
		}
		break;

	/* buffer 16-bit aligned */
	case 0x02:
		*(x_u16 *)p = cpu_to_le16(readw((x_sys)io));
		p += 2;
		count--;
		while(count>=2)
		{
			count -= 2;
			l = cpu_to_le16(readw((x_sys)io)) << 16;
			l |= cpu_to_le16(readw((x_sys)io));
			*(x_u16 *)p = l;
			p += 4;
		}
		if(count)
		{
			*(x_u16 *)p = cpu_to_le16(readw((x_sys)io));
		}
		break;

	/* buffer 8-bit aligned */
	case 0x01:
	case 0x03:
		--count;
		l = cpu_to_le16(readw((x_sys)io));
		*p = l >> 8;
		p++;
		while(count--)
		{
			l2 = cpu_to_le16(readw((x_sys)io));
			*(x_u16 *)p = (l & 0xff) << 8 | (l2 >> 8);
			p += 2;
			l = l2;
		}
		*p = l & 0xff;
		break;
	}
}

/*
 * read 32-bit words from io to memory
 */
void readsl(const void * io, void * dst, x_s32 count)
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
			*(x_u32 *)p = cpu_to_le32(readl((x_sys)io));
			p += 4;
		}
		break;

	/* buffer 16-bit aligned */
	case 0x02:
		--count;
		l = cpu_to_le32(readl((x_sys)io));
		*(x_u16 *)p = l >> 16;
		p += 2;

		while(count--)
		{
			l2 = cpu_to_le32(readl((x_sys)io));
			*(x_u32 *)p = (l & 0xffff) << 16 | (l2 >> 16);
			p += 4;
			l = l2;
		}
		*(x_u16 *)p = l & 0xffff;
		break;

	/* buffer 8-bit aligned */
	case 0x01:
		--count;
		l = cpu_to_le32(readl((x_sys)io));
		*(x_u8 *)p = l >> 24;
		p++;
		*(x_u16 *)p = (l >> 8) & 0xffff;
		p += 2;
		while(count--)
		{
			l2 = cpu_to_le32(readl((x_sys)io));
			*(x_u32 *)p = (l & 0xff) << 24 | (l2 >> 8);
			p += 4;
			l = l2;
		}
		*p = l & 0xff;
		break;

	/* buffer 8-bit aligned */
	case 0x03:
		--count;
		l = cpu_to_le32(readl((x_sys)io));
		*p = l >> 24;
		p++;
		while(count--)
		{
			l2 = cpu_to_le32(readl((x_sys)io));
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

/*
 * write 8-bit bytes from memory to io
 */
void writesb(void * io, const void * src, x_s32 count)
{
	const x_u8 * p = (const x_u8 *)src;

	while(count > 0)
	{
		count--;
		writeb((x_sys)io, *p);
		p++;
	}
}

/*
 * write 16-bit half words from memory to io
 */
void writesw(void * io, const void * src, x_s32 count)
{
	x_u32 l = 0, l2;
	const x_u8 * p = (const x_u8 *)src;

	if(count <= 0)
		return;

	switch(((x_u32)p) & 0x3)
	{
	/* buffer 32-bit aligned */
	case 0x00:
		while(count>=2)
		{
			count -= 2;
			l = *(x_u32 *)p;
			p += 4;
			writew((x_sys)io, le16_to_cpu(l >> 16));
			writew((x_sys)io, le16_to_cpu(l & 0xffff));
		}
		if(count > 0)
		{
			writew((x_sys)io, le16_to_cpu(*(x_u16*)p));
		}
		break;

	/* buffer 16-bit aligned */
	case 0x02:
		writew((x_sys)io, le16_to_cpu(*(x_u16*)p));
		p += 2;
		count--;

		while(count>=2)
		{
			count -= 2;
			l = *(x_u32 *)p;
			p += 4;
			writew((x_sys)io, le16_to_cpu(l >> 16));
			writew((x_sys)io, le16_to_cpu(l & 0xffff));
		}
		if(count > 0)
		{
			writew((x_sys)io, le16_to_cpu(*(x_u16 *)p));
		}
		break;

	/* buffer 8-bit aligned */
	case 0x01:
	case 0x03:
		l  = *p << 8;
		p++;
		count--;
		while(count > 0)
		{
			count--;
			l2 = *(x_u16 *)p;
			p += 2;
			writew((x_sys)io, le16_to_cpu(l | l2 >> 8));
		        l = l2 << 8;
		}
		l2 = *(x_u8 *)p;
		writew((x_sys)io, le16_to_cpu(l | l2>>8));
		break;
	}
}

/*
 * write 32-bit words from memory to io
 */
void writesl(void * io, const void * src, x_s32 count)
{
	x_u32 l = 0, l2;
	const x_u8 * p = (const x_u8 *)src;

	if(count <= 0)
		return;

	switch(((x_u32)p) & 0x3)
	{
	/* buffer 32-bit aligned */
	case 0x00:
		while(count--)
		{
			writel((x_sys)io, le32_to_cpu(*(x_u32 *)p));
			p += 4;
		}
		break;

	/* buffer 16-bit aligned */
	case 0x02:
		--count;
		l = *(x_u16 *)p;
		p += 2;

		while(count--)
		{
			l2 = *(x_u32 *)p;
			p += 4;
			writel((x_sys)io, le32_to_cpu(l << 16 | l2 >> 16));
			l = l2;
		}
		l2 = *(x_u16 *)p;
		writel((x_sys)io, le32_to_cpu(l << 16 | l2));
		break;

	/* buffer 8-bit aligned */
	case 0x01:
		--count;
		l = *p << 24;
		p++;
		l |= *(x_u16 *)p << 8;
		p += 2;

		while(count--)
		{
			l2 = *(x_u32 *)p;
			p += 4;
			writel((x_sys)io, le32_to_cpu(l | l2 >> 24));
			l = l2 << 8;
		}
		l2 = *p;
		writel((x_sys)io, le32_to_cpu(l | l2));
		break;

	/* buffer 32-bit aligned */
	case 0x03:
		--count;
		l = *p << 24;
		p++;

		while(count--)
		{
			l2 = *(x_u32 *)p;
			p += 4;
			writel((x_sys)io, le32_to_cpu(l | l2 >> 8));
			l = l2 << 24;
		}
		l2 = *(x_u16 *)p << 16;
		p += 2;
		l2 |= *p;
		writel((x_sys)io, le32_to_cpu(l | l2));
		break;
	}
}
