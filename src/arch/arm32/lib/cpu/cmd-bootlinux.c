/*
 * arch/arm32/lib/cpu/cmd-bootlinux.c
 *
 * Copyright(c) 2007-2015 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
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

#include <xboot/linux.h>
#include <command/command.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    bootlinux <KERNEL ADDR> <PARAM ADDR> <MACH TYPE> <COMMAND LINE>\r\n");
}

/*
 * boot and execute the linux kernel
 * r0 = must contain a zero or else the kernel loops
 * r1 = architecture type
 * r2 = physical address of tagged list in system ram
 */
static int do_bootlinux(int argc, char ** argv)
{
	s32_t ret;
	s32_t linux_mach_type, linux_kernel, linux_tag_placement;
	struct machine_t * mach = get_machine();
	struct tag * params;
	s8_t *p;
	s32_t i;

	if(argc != 5)
	{
		usage();
		return -1;
	}

	if(!mach)
	{
		printf("Can not get machine information.\r\n");
		return -1;
	}

	linux_kernel = strtoul((const char *)argv[1], NULL, 0);
	linux_tag_placement = strtoul((const char *)argv[2], NULL, 0);
	linux_mach_type = strtoul((const char *)argv[3], NULL, 0);

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
/*	for(i = 0; i < ARRAY_SIZE(mach->banks); i++)
	{
		if( (mach->banks[i].start == 0) && (mach->banks[i].size == 0) )
			break;

		params->hdr.tag = ATAG_MEM;
		params->hdr.size = tag_size(tag_mem32);
		params->u.mem.start = (u32_t)mach->banks[i].start;
		params->u.mem.size = (u32_t)mach->banks[i].size;
		params = tag_next(params);
	}
*/
	/* command line tags */
	p = (s8_t *)argv[4];
	if(p && strlen((const char *)p))
	{
		params->hdr.tag = ATAG_CMDLINE;
		params->hdr.size = (sizeof (struct tag_header) + strlen((char *)p) + 1 + 4) >> 2;
		strcpy((char *)(params->u.cmdline.cmdline), (char *)p);
		params = tag_next (params);
	}

	/* needs to always be the last tag */
	params->hdr.tag = ATAG_NONE;
	params->hdr.size = 0;

	/* now, booting linux */
	printf("Kernel address: 0x%08lx, param address: 0x%08lx, machine type: %d\r\n", linux_kernel, linux_tag_placement, linux_mach_type);
	printf("Now, booting linux......\r\n");

	/* clean up before run linux */
	machine_cleanup();

	/* go linux ... */
	ret = ((s32_t(*)(s32_t, s32_t, s32_t))(linux_kernel)) (0, linux_mach_type, linux_tag_placement);

	return ret;
}

static struct command_t cmd_bootlinux = {
	.name	= "bootlinux",
	.desc	= "boot and execute linux kernel",
	.usage	= usage,
	.exec	= do_bootlinux,
};

static __init void bootlinux_cmd_init(void)
{
	register_command(&cmd_bootlinux);
}

static __exit void bootlinux_cmd_exit(void)
{
	unregister_command(&cmd_bootlinux);
}

command_initcall(bootlinux_cmd_init);
command_exitcall(bootlinux_cmd_exit);
