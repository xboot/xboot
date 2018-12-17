/*
 * kernel/command/cmd-cp.c
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
