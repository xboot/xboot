/*
 * sys-tzpc.c
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

void sys_tzpc_init(void)
{
	virtual_addr_t addr;

	addr = 0xf1500000;
	write32(addr, 0);

	addr = 0xf1500000;
	write32(addr + 0x804, 0xff);
	write32(addr + 0x810, 0xff);
	write32(addr + 0x81c, 0xff);

	addr = 0xfad00000;
	write32(addr + 0x804, 0xff);
	write32(addr + 0x810, 0xff);
	write32(addr + 0x81c, 0xff);

	addr = 0xe0600000;
	write32(addr + 0x804, 0xff);
	write32(addr + 0x810, 0xff);
	write32(addr + 0x81c, 0xff);
	write32(addr + 0x828, 0xff);

	addr = 0xe1c00000;
	write32(addr + 0x804, 0xff);
	write32(addr + 0x810, 0xff);
	write32(addr + 0x81c, 0xff);
}
