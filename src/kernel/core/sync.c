/*
 * kernel/core/sync.c
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
#include <nvmem/nvmem.h>
#include <xboot/sync.h>

void sync(void)
{
	struct device_t * pos, * n;

	/*
	 * Nvmem sync
	 */
	list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_NVMEM], head)
	{
		nvmem_sync((struct nvmem_t *)(pos->priv));
	}

	/*
	 * Setting sync
	 */
	setting_sync();

	/*
	 * Vfs sync
	 */
	vfs_sync();

	/*
	 * Block sync
	 */
	list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_BLOCK], head)
	{
		block_sync((struct block_t *)(pos->priv));
	}
}
