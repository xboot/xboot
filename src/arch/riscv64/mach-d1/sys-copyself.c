/*
 * sys-copyself.c
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

extern unsigned char __image_start[];
extern unsigned char __image_end[];
extern unsigned char __heap_start[];
extern void sys_uart_putc(char c);
extern void sys_decompress(char * src, int slen, char * dst, int dlen);
extern void sys_crypt(char * key, char * buf, int len);
extern int sys_hash_keygen(char * msg, void * key);
extern int sys_hash(char * id, char * buf, int len, char * sha256);
extern int sys_verify(char * public, char * sha256, char * signature);
extern void sys_spinor_init(void);
extern void sys_spinor_exit(void);
extern void sys_spinor_read(int addr, void * buf, int count);

struct zdesc_t {			/* Total 256 bytes */
	uint8_t magic[4];		/* ZB??, I for bind id, E for encrypt image */
	uint8_t key[32];		/* Aes256 encrypt key (hardcode or efuse suggested) */
	uint8_t sha256[32];		/* Sha256 hash */
	uint8_t signature[64];	/* Ecdsa256 signature of sha256 */
	uint8_t csize[4];		/* Compress size of image */
	uint8_t dsize[4];		/* Decompress size of image */
	uint8_t public[33];		/* Ecdsa256 public key (hardcode suggested) */
	uint8_t majoy;			/* Majoy version */
	uint8_t minior;			/* Minior version */
	uint8_t patch;			/* Patch version */
	uint8_t message[80];	/* Message additionally */
};

enum {
	BOOT_DEVICE_SPINOR	= 1,
	BOOT_DEVICE_SPINAND	= 2,
	BOOT_DEVICE_SDCARD	= 3,
};

static int get_boot_device(void)
{
	return BOOT_DEVICE_SPINOR;
}

static void hexstr(char * str, unsigned char * buf, int len)
{
	char c;
	int i;

	for(i = 0; i < len; i++)
	{
		c = (buf[i] >> 4) & 0xf;
		if(c >= 10)
			*str++ = c - 10 + 'a';
		else
			*str++ = c + '0';
		c = (buf[i] >> 0) & 0xf;
		if(c >= 10)
			*str++ = c - 10 + 'a';
		else
			*str++ = c + '0';
	}
	*str = '\0';
}

void sys_copyself(void)
{
	int d = get_boot_device();
	void * mem, * tmp;
	uint32_t size;
	struct zdesc_t * z;
	uint32_t csize, dsize;
	char uniqueid[33];
	uint32_t sid[4];
	int sum, i;

	sid[0] = cpu_to_be32(read32(0x03006200 + 0x0));
	sid[1] = cpu_to_be32(read32(0x03006200 + 0x4));
	sid[2] = cpu_to_be32(read32(0x03006200 + 0x8));
	sid[3] = cpu_to_be32(read32(0x03006200 + 0xc));
	hexstr(uniqueid, (unsigned char *)sid, 16);

	if(d == BOOT_DEVICE_SPINOR)
	{
		z = (struct zdesc_t *)__heap_start;
		mem = (void *)__image_start;
		tmp = (void *)z + sizeof(struct zdesc_t);
		size = __image_end - __image_start;

		sys_spinor_init();
		sys_spinor_read(65536, z, sizeof(struct zdesc_t));
		sys_spinor_exit();
		if((z->magic[0] == 'Z') && (z->magic[1] == 'B') && ((z->magic[2] == 'I') || (z->magic[2] == 0)) && ((z->magic[3] == 'E') || (z->magic[3] == 0)))
		{
			for(sum = 0, i = 0; i < 32; i++)
				sum += z->key[i];
			if(sum == 0)
				sys_hash_keygen(uniqueid, z->key);
			sys_crypt((char *)z->key, (char *)z->sha256, sizeof(struct zdesc_t) - 36);
			if(sys_verify((char *)z->public, (char *)z->sha256, (char *)z->signature))
			{
				csize = (z->csize[0] << 24) | (z->csize[1] << 16) | (z->csize[2] << 8) | (z->csize[3] << 0);
				dsize = (z->dsize[0] << 24) | (z->dsize[1] << 16) | (z->dsize[2] << 8) | (z->dsize[3] << 0);
				sys_spinor_init();
				sys_spinor_read(65536 + sizeof(struct zdesc_t), tmp, csize);
				sys_spinor_exit();
				if(sys_hash((z->magic[2] == 'I') ? uniqueid : NULL, (char *)z->csize, sizeof(struct zdesc_t) - 132 + csize, (char *)z->sha256))
				{
					if(z->magic[3] == 'E')
						sys_crypt((char *)z->key, tmp, csize);
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
	else if(d == BOOT_DEVICE_SPINAND)
	{
	}
	else if(d == BOOT_DEVICE_SDCARD)
	{
	}
}
