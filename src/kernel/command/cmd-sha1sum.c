/*
 * kernel/command/cmd-sha1sum.c
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
#include <sha1.h>
#include <command/command.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    sha1sum <file> ...\r\n");
}

static int sha1sum_file(const char * filename)
{
	struct sha1_ctx_t ctx;
	struct vfs_stat_t st;
	const uint8_t * digest;
	char fpath[VFS_MAX_PATH];
	char * buf;
	u64_t n;
	int fd, i;

	if(shell_realpath(filename, fpath) < 0)
	{
		printf("sha1sum: %s: Can not convert to realpath\r\n", filename);
		return -1;
	}

	if(vfs_stat(fpath, &st) < 0)
	{
		printf("sha1sum: %s: No such file or directory\r\n", fpath);
		return -1;
	}

	if(S_ISDIR(st.st_mode))
	{
		printf("sha1sum: %s: Is a directory\r\n", fpath);
		return -1;
	}

	fd = vfs_open(fpath, O_RDONLY, 0);
	if(fd < 0)
	{
		printf("sha1sum: %s: Can not open\r\n", fpath);
		return -1;
	}

	buf = malloc(SZ_64K);
	if(!buf)
	{
		printf("sha1sum: Can not alloc memory\r\n");
		vfs_close(fd);
		return -1;
	}

	sha1_init(&ctx);
	while((n = vfs_read(fd, buf, SZ_64K)) > 0)
	{
		sha1_update(&ctx, buf, n);
	}
	digest = sha1_final(&ctx);
	for(i = 0; i < SHA1_DIGEST_SIZE; i++)
		printf("%02x", digest[i]);
	printf(" %s\r\n", fpath);

	free(buf);
	vfs_close(fd);

	return 0;
}

static int do_sha1sum(int argc, char ** argv)
{
	int i;

	if(argc == 1)
	{
		usage();
		return -1;
	}
	for(i = 1; i < argc; i++)
	{
		if(sha1sum_file(argv[i]) != 0)
			return -1;
	}
	return 0;
}

static struct command_t cmd_sha1sum = {
	.name	= "sha1sum",
	.desc	= "compute sha1 message digest",
	.usage	= usage,
	.exec	= do_sha1sum,
};

static __init void sha1sum_cmd_init(void)
{
	register_command(&cmd_sha1sum);
}

static __exit void sha1sum_cmd_exit(void)
{
	unregister_command(&cmd_sha1sum);
}

command_initcall(sha1sum_cmd_init);
command_exitcall(sha1sum_cmd_exit);
