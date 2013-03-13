/*
 * kernel/command/cmd_cp.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
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

#include <xboot.h>
#include <types.h>
#include <string.h>
#include <malloc.h>
#include <xboot/log.h>
#include <xboot/list.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <command/command.h>


#if	defined(CONFIG_COMMAND_CP) && (CONFIG_COMMAND_CP > 0)

static int do_cp(int argc, char ** argv)
{
	//FIXME
	/*
	s8_t buf[128];
	s32_t fd1, fd2;
	s64_t done, wrote;

	if(argc != 3)
	{
		printk("usage:\r\n    cp SOURCE DEST\r\n");
		return -1;
	}

    if(access((const char *)argv[1], F_OK) != 0)
    {
    	printk("1\r\n");
    }

    if(access((const char *)argv[2], F_OK) != 0)
    {
    	printk("2\r\n");
    }

    fd1 = open((const char *)argv[1], O_RDONLY);
    if(fd1 < 0)
    	printk("3\r\n");

    fd2 = open((const char *)argv[2], O_WRONLY|O_CREAT );
    if( fd2 < 0 )
    	printk("4\r\n");

    for(;;)
    {
        done = read( fd1, buf, 128);

        if( done == 0 ) break;

        wrote = write( fd2, buf, done );

        if( wrote != done )
        	break;
    }

    close( fd1 );
    close( fd2 );
*/
	return 0;
}

static struct command cp_cmd = {
	.name		= "cp",
	.func		= do_cp,
	.desc		= "copy file\r\n",
	.usage		= "cp SOURCE DEST\r\n",
	.help		= "    copy SOURCE to DEST.\r\n"
};

static __init void cp_cmd_init(void)
{
	if(!command_register(&cp_cmd))
		LOG_E("register 'cp' command fail");
}

static __exit void cp_cmd_exit(void)
{
	if(!command_unregister(&cp_cmd))
		LOG_E("unregister 'cp' command fail");
}

command_initcall(cp_cmd_init);
command_exitcall(cp_cmd_exit);

#endif
