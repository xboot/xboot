/*
 * kernel/time/delay.c
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
#include <time/delay.h>

void ndelay(u32_t ns)
{
	ktime_t timeout = ktime_add_ns(ktime_get(), ns);
	while(ktime_before(ktime_get(), timeout));
}
EXPORT_SYMBOL(ndelay);

void udelay(u32_t us)
{
	ktime_t timeout = ktime_add_us(ktime_get(), us);
	while(ktime_before(ktime_get(), timeout));
}
EXPORT_SYMBOL(udelay);

void mdelay(u32_t ms)
{
	ktime_t timeout = ktime_add_ms(ktime_get(), ms);
	while(ktime_before(ktime_get(), timeout));
}
EXPORT_SYMBOL(mdelay);
