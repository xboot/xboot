/*
 * sys-copyself.c
 *
 * Copyright(c) 2007-2022 Jianjun Jiang <8192542@qq.com>
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

extern unsigned char __image_start[];
extern unsigned char __image_end[];
extern unsigned char __heap_start[];
extern void return_to_fel(void);
extern void sys_mmu_init(void);
extern void sys_uart_putc(char c);
extern void sys_decompress(char * src, int slen, char * dst, int dlen);
extern int sys_hash(char * buf, int len, char * sha256);
extern int sys_verify(char * public, char * sha256, char * signature);
extern void sys_spinor_init(void);
extern void sys_spinor_exit(void);
extern void sys_spinor_read(int addr, void * buf, int count);

struct zdesc_t {			/* Total 256 bytes */
	uint8_t magic[4];		/* ZBL! */
	uint8_t signature[64];	/* Ecdsa256 signature of sha256 */
	uint8_t sha256[32];		/* Sha256 hash */
	uint8_t majoy;			/* Majoy version */
	uint8_t minior;			/* Minior version */
	uint8_t patch;			/* Patch version */
	uint8_t csize[4];		/* Compress size of image */
	uint8_t dsize[4];		/* Decompress size of image */
	uint8_t pubkey[33];		/* Ecdsa256 public key */
	uint8_t message[112];	/* Message additionally */
};

enum {
	BOOT_DEVICE_FEL	= 0,
	BOOT_DEVICE_SPI	= 1,
	BOOT_DEVICE_MMC	= 2,
};

static int get_boot_device(void)
{
	u32_t * t = (void *)0x00000058;

	if(t[0] == 0x1)
		return BOOT_DEVICE_FEL;
	return BOOT_DEVICE_SPI;
}

void sys_copyself(void)
{
	int d = get_boot_device();

	if(d == BOOT_DEVICE_FEL)
	{
		sys_uart_putc('B');
		sys_uart_putc('o');
		sys_uart_putc('o');
		sys_uart_putc('t');
		sys_uart_putc(' ');
		sys_uart_putc('t');
		sys_uart_putc('o');
		sys_uart_putc(' ');
		sys_uart_putc('F');
		sys_uart_putc('E');
		sys_uart_putc('L');
		sys_uart_putc(' ');
		sys_uart_putc('m');
		sys_uart_putc('o');
		sys_uart_putc('d');
		sys_uart_putc('e');
		sys_uart_putc('\r');
		sys_uart_putc('\n');
		return_to_fel();
	}
	else if(d == BOOT_DEVICE_SPI)
	{
		struct zdesc_t * z = (struct zdesc_t *)__heap_start;
		void * mem = (void *)__image_start;
		void * tmp = (void *)z + sizeof(struct zdesc_t);
		uint32_t size = __image_end - __image_start;

		sys_mmu_init();
		sys_spinor_init();
		sys_spinor_read(24576, z, sizeof(struct zdesc_t));
		sys_spinor_exit();
		if((z->magic[0] == 'Z') && (z->magic[1] == 'B') && (z->magic[2] == 'L') && (z->magic[3] == '!'))
		{
			//if(sys_verify((char *)z->pubkey, (char *)z->sha256, (char *)z->signature))
			{
				uint32_t csize = (z->csize[0] << 24) | (z->csize[1] << 16) | (z->csize[2] << 8) | (z->csize[3] << 0);
				uint32_t dsize = (z->dsize[0] << 24) | (z->dsize[1] << 16) | (z->dsize[2] << 8) | (z->dsize[3] << 0);
				sys_spinor_init();
				sys_spinor_read(24576 + sizeof(struct zdesc_t), tmp, csize);
				sys_spinor_exit();
				//if(sys_hash((char *)(&z->majoy), (sizeof(struct zdesc_t) - 100) + csize, (char *)z->sha256))
				{
					sys_decompress(tmp, csize, mem, dsize);
				}
			}
		}
		else
		{
			sys_spinor_init();
			sys_spinor_read(0, mem, size);
			sys_spinor_exit();
		}
	}
	else if(d == BOOT_DEVICE_MMC)
	{
	}
}
