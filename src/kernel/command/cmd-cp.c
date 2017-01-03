/*
 * kernel/command/cmd-cp.c
 *
 * Copyright(c) 2007-2017 Jianjun Jiang <8192542@qq.com>
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

#include <command/command.h>

#if	0
static void usage(void)
{
	printf("usage:\r\n");
	printf("    cp ...\r\n");
}

static int do_cp(int argc, char ** argv)
{
	//FIXME
	/*
	s8_t buf[128];
	s32_t fd1, fd2;
	s64_t done, wrote;

	if(argc != 3)
	{
		printf("usage:\r\n    cp SOURCE DEST\r\n");
		return -1;
	}

    if(access((const char *)argv[1], F_OK) != 0)
    {
    	printf("1\r\n");
    }

    if(access((const char *)argv[2], F_OK) != 0)
    {
    	printf("2\r\n");
    }

    fd1 = open((const char *)argv[1], O_RDONLY);
    if(fd1 < 0)
    	printf("3\r\n");

    fd2 = open((const char *)argv[2], O_WRONLY|O_CREAT );
    if( fd2 < 0 )
    	printf("4\r\n");

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

static struct command_t cmd_cp = {
	.name	= "cp",
	.desc	= "copy file",
	.usage	= usage,
	.exec	= do_cp,
};

static __init void cp_cmd_init(void)
{
	register_command(&cmd_cp);
}

static __exit void cp_cmd_exit(void)
{
	unregister_command(&cmd_cp);
}

command_initcall(cp_cmd_init);
command_exitcall(cp_cmd_exit);

#endif
