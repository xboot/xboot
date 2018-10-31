/*
 * init/runtime.c
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

#include <xfs/xfs.h>
#include <runtime.h>

static struct runtime_t * __current_runtime = NULL;

struct runtime_t * runtime_get(void)
{
	return __current_runtime;
}

void runtime_create_save(struct runtime_t * rt, const char * path, struct runtime_t ** r)
{
	if(!rt)
		return;

	if(r)
		*r = __current_runtime;
	__current_runtime = rt;

	rt->__stdin = __file_alloc(0);
	rt->__stdout = __file_alloc(1);
	rt->__stderr = __file_alloc(2);

	rt->__event_base = __event_base_alloc();
	rt->__xfs_ctx = __xfs_alloc(path);
}

void runtime_destroy_restore(struct runtime_t * rt, struct runtime_t * r)
{
	if(!rt)
		return;

	if(rt->__xfs_ctx)
		__xfs_free(rt->__xfs_ctx);

	if(rt->__event_base)
		__event_base_free(rt->__event_base);

	if(rt->__stderr)
		fclose(rt->__stderr);

	if(rt->__stdout)
		fclose(rt->__stdout);

	if(rt->__stdin)
		fclose(rt->__stdin);

	if(r)
		__current_runtime = r;
}
