/*
 * cmd-bootlinux.c
 */

#include <command/command.h>

struct image_header_t {
	uint32_t code0;			/* Executable code */
	uint32_t code1;			/* Executable code */
	uint64_t text_offset;	/* Image load offset, LE */
	uint64_t image_size;	/* Effective Image size, LE */
	uint64_t flags;			/* Kernel flags, LE */
	uint64_t res2;			/* reserved */
	uint64_t res3;			/* reserved */
	uint64_t res4;			/* reserved */
	uint32_t magic;			/* Magic number */
	uint32_t res5;
};

static void usage(void)
{
	printf("usage:\r\n");
	printf("    bootlinux <kernel address> <dtb address>\r\n");
}

static int do_bootlinux(int argc, char ** argv)
{
	struct machine_t * mach = get_machine();
	struct image_header_t * h;
	uint64_t image_size, text_offset, dst;

	void * kernel;
	void * dtb;
	void * e;

	if(!mach || argc != 3)
	{
		usage();
		return -1;
	}

	kernel = (void (*)(void *, void *, void *, void *))strtoull(argv[1], NULL, 0);
	dtb = (void *)strtoull(argv[2], NULL, 0);

	h = (struct image_header_t *)kernel;
	if(h->magic != le32_to_cpu(0x644d5241))
	{
		printf("Bad arm64 linux kernel image\r\n");
		return -1;
	}
	if(h->image_size == 0)
	{
		printf("Image lacks image_size field, assuming 16MiB\r\n");
		image_size = 16 << 20;
		text_offset = 0x80000;
	}
	else
	{
		image_size = le64_to_cpu(h->image_size);
		text_offset = le64_to_cpu(h->text_offset);
	}
	if(le64_to_cpu(h->flags) & (1 << 3))
		dst = (uint64_t)kernel - text_offset;
	else
		dst = (uint64_t)mach->map[0].virt;
	e = (void *)(((dst + 0x001fffffULL) &~ 0x001fffffULL) + text_offset);
	if(e != kernel)
	{
		printf("Moving Image from 0x%016llx to 0x%016llx\r\n", kernel, e);
		memmove(e, kernel, image_size);
	}

	/* Now, booting linux */
	printf("Kernel address: 0x%016llx, dtb address: 0x%016llx\r\n", (uint64_t)e, (uint64_t)dtb);
	printf("Now, booting linux ......\r\n");

	machine_cleanup();
	((void (*)(void * dtb, void * res0, void * res1, void * res2))e)(dtb, 0, 0, 0);

	return 0;
}

static struct command_t cmd_bootlinux = {
	.name	= "bootlinux",
	.desc	= "boot arm64 linux kernel image stored in memory",
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
