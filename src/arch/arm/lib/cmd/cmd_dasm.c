/*
 * arch/arm/lib/cmd/cmd_dasm.c
 *
 * Copyright (c) 2007-2010  jianjun jiang <jerryjianjun@gmail.com>
 * website: http://xboot.org
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
#include <malloc.h>
#include <vsprintf.h>
#include <xboot/log.h>
#include <xboot/io.h>
#include <xboot/list.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <terminal/terminal.h>
#include <terminal/curses.h>
#include <shell/command.h>
#include <fs/fsapi.h>
#include <disassembler.h>


#if	defined(CONFIG_COMMAND_DASM) && (CONFIG_COMMAND_DASM > 0)

static x_s32 dasm(x_s32 argc, const x_s8 **argv)
{
	x_bool thumb = FALSE;
	x_u32 address = 0x00000000;
	x_u32 count = 1;
	char * filename = NULL;
	struct arm_instruction instruction;
	x_s32 fd;
	x_s8 buf[256];
	x_s32 len, i;

	if(argc < 2)
	{
		printk("usage:\r\n    dasm <address> [-a|-t] [-c count] [-f file]\r\n");
		return (-1);
	}

	address = simple_strtou32(argv[1], NULL, 0);

	for(i=2; i<argc; i++)
	{
		if( !strcmp(argv[i],(x_s8*)"-a") )
			thumb = FALSE;
		else if( !strcmp(argv[i],(x_s8*)"-t") )
			thumb = TRUE;
		else if( !strcmp(argv[i],(x_s8*)"-c") && (argc > i+1))
		{
			count = simple_strtou32(argv[i+1], NULL, 0);
			i++;
		}
		else if( !strcmp(argv[i],(x_s8*)"-f") && (argc > i+1))
		{
			filename = (char *)argv[i+1];
			i++;
		}
		else
		{
			printk("dasm: invalid option '%s'\r\n", argv[i]);
			printk("usage:\r\n    dasm [-a|-t] <address> [-c count] [-f file]\r\n");
			printk("try 'help dasm' for more information.\r\n");
			return (-1);
		}
	}

	if(filename != NULL)
	{
		fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH));
		if(fd < 0)
		{
			printk("can not to create file '%s'\r\n", filename);
			return -1;
		}
	}

	while(count--)
	{
		if(thumb == TRUE)
			thumb_evaluate_opcode(readw(address), address, &instruction);
		else
			arm_evaluate_opcode(readl(address), address, &instruction);

		len = sprintf(buf, (const x_s8 *)"%s\r\n", instruction.text);

		if(filename)
		{
			if( write(fd, (void *)buf, len) != len )
			{
				close(fd);
				unlink(filename);
				printk("failed to write file '%s'\r\n", filename);
			}
		}
		else
		{
			printk((const char *)buf);
		}

		address += instruction.instruction_size;
	}

	if(filename != NULL)
		close(fd);

	return 0;
}

static struct command dasm_cmd = {
	.name		= "dasm",
	.func		= dasm,
	.desc		= "arm instruction disassembler\r\n",
	.usage		= "dasm <address> [-a|-t] [-c count] [-f file]\r\n",
	.help		= "    disassemble tool for arm instruction\r\n"
				  "    -a    disassemble with arm instruction (default for arm)\r\n"
				  "    -t    disassemble with thumb instruction\r\n"
				  "    -c    the count of instruction (default is one instruction)\r\n"
				  "    -f    save to file with result\r\n"
};

static __init void dasm_cmd_init(void)
{
	if(!command_register(&dasm_cmd))
		LOG_E("register 'dasm' command fail");
}

static __exit void dasm_cmd_exit(void)
{
	if(!command_unregister(&dasm_cmd))
		LOG_E("unregister 'dasm' command fail");
}

module_init(dasm_cmd_init, LEVEL_COMMAND);
module_exit(dasm_cmd_exit, LEVEL_COMMAND);

#endif
