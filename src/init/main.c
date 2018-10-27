/*
 * init/main.c
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
#include <init.h>
#include <dma/dma.h>
#include <shell/shell.h>

int xboot_main(int argc, char * argv[])
{
	struct runtime_t rt;
	struct task_t * task;

	/* Do initial mem pool */
	do_init_mem_pool();

	/* Do initial dma pool */
	do_init_dma_pool();

	/* Do initial vfs */
	do_init_vfs();

	/* Create runtime */
	runtime_create_save(&rt, 0, 0);

	/* Do all initial calls */
	do_initcalls();

	/* Do show logo */
	do_showlogo();

	/* Do auto boot */
	//do_autoboot();

	/* Create shell task */
	task = task_create(scheduler_self(), "shell", task_shell, 0, 0, 0);

	/* Scheduler loop */
	scheduler_loop();

	/* Destory shell task */
	task_destory(task);

	/* Do all exit calls */
	do_exitcalls();

	/* Destroy runtime */
	runtime_destroy_restore(&rt, 0);

	/* Xboot return */
	return 0;
}
