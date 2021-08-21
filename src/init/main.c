/*
 * init/main.c
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
#include <init.h>

static void init_task(struct task_t * task, void * data)
{
	/* Do initial vfs */
	do_init_vfs();

	/* Do initial calls */
	do_initcalls();

	/* Do initial setting */
	do_init_setting();

	/* Do show logo */
	do_show_logo();

	/* Do play audio */
	do_play_audio();

	/* Do auto mount */
	do_auto_mount();

	/* Do shell task */
	do_shell_task();

	/* Do auto boot */
	do_auto_boot();
}

void xboot_main(void)
{
	/* Do initial memory */
	do_init_mem();

	/* Do initial scheduler */
	do_init_sched();

	/* Create and resume init task */
	task_resume(task_create(scheduler_self(), "init", init_task, NULL, 0, 0));

	/* Scheduler loop */
	scheduler_loop();
}
