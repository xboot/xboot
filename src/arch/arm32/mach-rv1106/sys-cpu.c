/*
 * sys-cpu.c
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

static inline void sdelay(int loops)
{
	__asm__ __volatile__ ("1:\n" "subs %0, %1, #1\n"
		"bne 1b":"=r" (loops):"0"(loops));
}

void sys_cpu_init(void)
{
	/*
	 * Save chip version to OS_REG1[2:0]
	 */
	if(read32(0xffff4ffc) == 0x30303256)
		write32(0xff020204, (read32(0xff020204) & ~0x7) | 0x1);
	else
		write32(0xff020204, (read32(0xff020204) & ~0x7) | 0x0);

	/*
	 * Set all devices to Non-secure
	 */
	write32(0xff070000 + 0x0020, 0xffff0000);
	write32(0xff070000 + 0x0024, 0xffff0000);
	write32(0xff070000 + 0x0028, 0xffff0000);
	write32(0xff070000 + 0x002c, 0xffff0000);
	write32(0xff070000 + 0x0030, 0xffff0000);
	write32(0xff070000 + 0x00bc, 0x000f0000);
	write32(0xff076000 + 0x0020, 0xffff0000);
	write32(0xff076000 + 0x0024, 0xffff0000);
	write32(0xff076000 + 0x0028, 0xffff0000);
	write32(0xff076000 + 0x002c, 0xffff0000);
	write32(0xff076000 + 0x0030, 0xffff0000);
	write32(0xff076000 + 0x0040, 0x00030002);
	write32(0xff080000, 0x20000000);

	/*
	 * Set the emmc and fspi to access secure area
	 */
	write32(0xff900000 + 0x4c, 0x00000000);
	write32(0xff910000 + 0x44, 0xff00ffff);

	/*
	 * Set fspi clk 6mA
	 */
	if((read32(0xff568000 + 0x0008) & 0x70) == 0x20)
		write32(0xff568000 + 0x0030, 0x3f000700);

	/*
	 * Set the USB2 PHY in suspend mode and turn off the
	 * USB2 PHY FS/LS differential receiver to save power
	 */
	write32(0xff000000 + 0x0050, 0x01ff01d1);
	write32(0xff3e0000 + 0x0100, 0x00000000);

	/*
	 * Release the watchdog
	 */
	write32(0xff000000 + 0x0004, 0x2000200);
	write32(0xff3b0000 + 0x0c10, 0x400040);

	/*
	 * When venc/npu use pvtpll, reboot will fail, because
	 * pvtpll is reset before venc/npu reset, so venc/npu
	 * is not completely reset, system will block when access
	 * NoC in SPL.
	 */
	write32(0xff3b0000 + 0x1008, 0xffff0018);
	write32(0xff3b0000 + 0x1000, 0x00030003);
	write32(0xff3b0000 + 0x1038, 0xffff0018);
	write32(0xff3b0000 + 0x1030, 0x00030003);
	sdelay(20);

	/*
	 * Limits npu max transport packets to 4 for route to scheduler,
	 * give much more chance for other controllers to access memory.
	 */
	write32(0xff140080 + 0x0008, 0x4);

	/*
	 * Improve VENC QOS PRIORITY
	 */
	write32(0xff150000 + 0x0008, 0x303);
}
