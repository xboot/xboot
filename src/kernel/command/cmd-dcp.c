/*
 * kernel/command/cmd-dcp.c
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

enum devtype_t {
	DEVTYPE_BLOCK	= 0,
	DEVTYPE_FILE	= 1,
	DEVTYPE_MEM		= 2,
};

static void usage(void)
{
	printf("usage:\r\n");
	printf("    dcp <input@offset:size> <output@offset:size>\r\n");
}

static int do_dcp(int argc, char ** argv)
{
	uint64_t start = ktime_to_ns(ktime_get());
	uint64_t end;
	char sbyte[32];
	char sspeed[32];
	enum devtype_t itype, otype;
	struct block_t * iblk = NULL, * oblk = NULL;
	int ifd = -1, ofd = -1;
	char ipath[VFS_MAX_PATH];
	char opath[VFS_MAX_PATH];
	char * iname, * oname;
	u64_t ioff, isize;
	u64_t ooff, osize;
	s64_t n, s, l;
	char * buf;
	char * p, * offset, * size;

	if(argc != 3)
	{
		usage();
		return -1;
	}

	buf = malloc(SZ_64K);
	if(!buf)
		return -1;

	p = argv[1];
	iname = strsep(&p, "@");
	offset = strsep(&p, ":");
	size = p;
	iname = (iname && strcmp(iname, "") != 0) ? iname : NULL;
	offset = (offset && strcmp(offset, "") != 0) ? offset : NULL;
	size = (size && strcmp(size, "") != 0) ? size : NULL;
	ioff = offset ? strtoull(offset, NULL, 0) : 0;
	isize = size ? strtoull(size, NULL, 0) : ~0ULL;
	if(!iname)
	{
		itype = DEVTYPE_MEM;
	}
	else if((iblk = search_block(iname)))
	{
		l = block_capacity(iblk);
		if(ioff >= l)
		{
			printf("Offset too large of input device '%s'\r\n", iname);
			free(buf);
			return -1;
		}
		isize = isize < (l - ioff) ? isize : (l - ioff);
		if(isize <= 0)
		{
			printf("Don't need to copy of input device '%s'\r\n", iname);
			free(buf);
			return -1;
		}
		itype = DEVTYPE_BLOCK;
	}
	else if(((shell_realpath(iname, ipath)) >= 0) && ((ifd = vfs_open(ipath, O_RDONLY, 0)) > 0))
	{
		struct vfs_stat_t st;

		vfs_fstat(ifd, &st);
		l = st.st_size;
		if(ioff >= l)
		{
			printf("Offset too large of input file '%s'\r\n", iname);
			free(buf);
			return -1;
		}
		isize = isize < (l - ioff) ? isize : (l - ioff);
		if(isize <= 0)
		{
			printf("Don't need to copy of input file '%s'\r\n", iname);
			free(buf);
			return -1;
		}
		itype = DEVTYPE_FILE;
	}
	else
	{
		printf("Can't find any input device '%s'\r\n", iname);
		free(buf);
		return -1;
	}

	p = argv[2];
	oname = strsep(&p, "@");
	offset = strsep(&p, ":");
	size = p;
	oname = (oname && strcmp(oname, "") != 0) ? oname : NULL;
	offset = (offset && strcmp(offset, "") != 0) ? offset : NULL;
	size = (size && strcmp(size, "") != 0) ? size : NULL;
	ooff = offset ? strtoull(offset, NULL, 0) : 0;
	osize = size ? strtoull(size, NULL, 0) : ~0ULL;
	if(!oname)
	{
		otype = DEVTYPE_MEM;
	}
	else if((oblk = search_block(oname)))
	{
		l = block_capacity(oblk);
		if(ooff >= l)
		{
			printf("Offset too large of output device '%s'\r\n", oname);
			free(buf);
			return -1;
		}
		osize = osize < (l - ooff) ? osize : (l - ooff);
		if(osize <= 0)
		{
			printf("Don't need copy to output device '%s'\r\n", oname);
			free(buf);
			return -1;
		}
		otype = DEVTYPE_BLOCK;
	}
	else if(((shell_realpath(oname, opath)) >= 0) && ((ofd = vfs_open(opath, (O_WRONLY | O_CREAT | O_TRUNC), 0644)) > 0))
	{
		ooff = 0;
		otype = DEVTYPE_FILE;
	}
	else
	{
		printf("Can't find any output device '%s'\r\n", oname);
		free(buf);
		return -1;
	}

	s = isize < osize ? isize : osize;
	l = 0;
	if(itype == DEVTYPE_BLOCK)
		block_sync(iblk);

	while(l < s)
	{
		n = (s - l) < SZ_64K ? (s - l) : SZ_64K;

		switch(itype)
		{
		case DEVTYPE_BLOCK:
			n = block_read(iblk, (u8_t *)buf, ioff + l, n);
			break;
		case DEVTYPE_FILE:
			n = vfs_read(ifd, buf, n);
			break;
		case DEVTYPE_MEM:
			memcpy(buf, (void *)((virtual_addr_t)(ioff + l)), n);
			break;
		default:
			break;
		}

		switch(otype)
		{
		case DEVTYPE_BLOCK:
			block_write(oblk, (u8_t *)buf, ooff + l, n);
			break;
		case DEVTYPE_FILE:
			vfs_write(ofd, buf, n);
			break;
		case DEVTYPE_MEM:
			memcpy((void *)((virtual_addr_t)(ooff + l)), buf, n);
			break;
		default:
			break;
		}

		l += n;
	}

	if(itype == DEVTYPE_FILE)
		vfs_close(ifd);
	if(otype == DEVTYPE_FILE)
		vfs_close(ofd);
	else if(otype == DEVTYPE_BLOCK)
		block_sync(oblk);
	free(buf);

	end = ktime_to_ns(ktime_get());
	printf("Copyed %s at %s/S - %s@0x%llx:0x%llx -> %s@0x%llx:0x%llx\r\n", ssize(sbyte, s), ssize(sspeed, (double)s * 1000000000.0 / (double)(end - start)), iname ? iname : "", ioff, s, oname ? oname : "", ooff, s);
	return 0;
}

static struct command_t cmd_dcp = {
	.name	= "dcp",
	.desc	= "device copy for file or block or memory",
	.usage	= usage,
	.exec	= do_dcp,
};

static __init void dcp_cmd_init(void)
{
	register_command(&cmd_dcp);
}

static __exit void dcp_cmd_exit(void)
{
	unregister_command(&cmd_dcp);
}

command_initcall(dcp_cmd_init);
command_exitcall(dcp_cmd_exit);
