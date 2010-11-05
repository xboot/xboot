/*
 * kernel/command/cmd_bootlinux.c
 *
 * Copyright (c) 2007-2008  jianjun jiang <jjjstudio@gmail.com>
 * official site: http://xboot.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <configs.h>
#include <default.h>
#include <types.h>
#include <string.h>
#include <configs.h>
#include <malloc.h>
#include <vsprintf.h>
#include <shell/env.h>
#include <xboot/log.h>
#include <xboot/list.h>
#include <xboot/linux.h>
#include <xboot/printk.h>
#include <xboot/machine.h>
#include <xboot/initcall.h>
#include <shell/command.h>


#if	defined(CONFIG_COMMAND_BOOTLINUX) && (CONFIG_COMMAND_BOOTLINUX > 0)

/*
 * boot and execute the linux kernel
 * r0 = must contain a zero or else the kernel loops
 * r1 = architecture type
 * r2 = physical address of tagged list in system ram
 */
static x_s32 bootlinux(x_s32 argc, const x_s8 **argv)
{
	x_s32 ret;
	x_s32 linux_mach_type, linux_kernel, linux_tag_placement;
	struct machine * mach = get_machine();
	struct tag * params;
	x_s8 *p;

	if(argc != 3)
	{
		printk("usage:\r\n    bootlinux <KERNEL ADDR> <PARAM ADDR>\r\n");
		return -1;
	}

	if(!mach)
	{
		printk("do not get machine information.\r\n");
		return -1;
	}

	linux_mach_type = simple_strtou32((x_s8*)env_get("linux-machtype"), NULL, 0);
	linux_kernel = simple_strtou32(argv[1], NULL, 0);
	linux_tag_placement = simple_strtou32(argv[2], NULL, 0);

	/* setup linux kernel boot params */
	params = (struct tag *)linux_tag_placement;

	/* first tag */
	params->hdr.tag = ATAG_CORE;
	params->hdr.size = tag_size(tag_core);
	params->u.core.flags = 0;
	params->u.core.pagesize = 0;
	params->u.core.rootdev = 0;
	params = tag_next(params);

	/* memory tags */
	params->hdr.tag = ATAG_MEM;
	params->hdr.size = tag_size(tag_mem32);
	params->u.mem.start = mach->res.mem_start;
	params->u.mem.size = mach->res.mem_end - mach->res.mem_start;
	params = tag_next(params);

	/* command line tags */
	p = (x_s8*)env_get("linux-cmdline");
	if(p && strlen(p))
	{
		params->hdr.tag = ATAG_CMDLINE;
		params->hdr.size = (sizeof (struct tag_header) + strlen(p) + 1 + 4) >> 2;
		strcpy((x_s8*)(params->u.cmdline.cmdline), p);
		params = tag_next (params);
	}

	/* needs to always be the last tag */
	params->hdr.tag = ATAG_NONE;
	params->hdr.size = 0;

	/* now, booting linux */
	printk("kernel address: 0x%08lx, param address: 0x%08lx, machine type: %d\r\n", linux_kernel, linux_tag_placement, linux_mach_type);
	printk("now, booting linux......\r\n");

	/* clean up before run linux */
	cleanup();

	/* go linux ... */
	ret = ((x_s32(*)(x_s32, x_s32, x_s32))(linux_kernel)) (0, linux_mach_type, linux_tag_placement);

	return ret;
}

static struct command bootlinux_cmd = {
	.name		= "bootlinux",
	.func		= bootlinux,
	.desc		= "boot and execute linux kernel\r\n",
	.usage		= "bootlinux <KERNEL ADDR> <PARAM ADDR>\r\n",
	.help		= "    boot and execute kernel with kernel address and linux params address\r\n"
};

static __init void bootlinux_cmd_init(void)
{
	if(!command_register(&bootlinux_cmd))
		LOG_E("register 'bootlinux' command fail");
}

static __exit void bootlinux_cmd_exit(void)
{
	if(!command_unregister(&bootlinux_cmd))
		LOG_E("unregister 'bootlinux' command fail");
}

module_init(bootlinux_cmd_init, LEVEL_COMMAND);
module_exit(bootlinux_cmd_exit, LEVEL_COMMAND);

#endif
