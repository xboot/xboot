#ifndef __LINUX_H__
#define __LINUX_H__


#include <configs.h>
#include <default.h>

/* The list ends with an ATAG_NONE node. */
#define ATAG_NONE		0x00000000

struct tag_header {
	x_u32 size;
	x_u32 tag;
};

/* The list must start with an ATAG_CORE node */
#define ATAG_CORE		0x54410001

struct tag_core {
	x_u32 flags;		/* bit 0 = read-only */
	x_u32 pagesize;
	x_u32 rootdev;
};

/* it is allowed to have multiple ATAG_MEM nodes */
#define ATAG_MEM		0x54410002

struct tag_mem32 {
	x_u32 size;
	x_u32 start;	/* physical start address */
};

/* VGA text type displays */
#define ATAG_VIDEOTEXT	0x54410003

struct tag_videotext {
	x_u8		x;
	x_u8		y;
	x_u16		video_page;
	x_u8		video_mode;
	x_u8		video_cols;
	x_u16		video_ega_bx;
	x_u8		video_lines;
	x_u8		video_isvga;
	x_u16		video_points;
};

/* describes how the ramdisk will be used in kernel */
#define ATAG_RAMDISK	0x54410004

struct tag_ramdisk {
	x_u32 flags;	/* bit 0 = load, bit 1 = prompt */
	x_u32 size;	/* decompressed ramdisk size in _kilo_ bytes */
	x_u32 start;	/* starting block of floppy-based RAM disk image */
};

/* describes where the compressed ramdisk image lives (virtual address) */
/*
 * this one accidentally used virtual addresses - as such,
 * it's deprecated.
 */
#define ATAG_INITRD		0x54410005

/* describes where the compressed ramdisk image lives (physical address) */
#define ATAG_INITRD2	0x54420005

struct tag_initrd {
	x_u32 start;	/* physical start address */
	x_u32 size;	/* size of compressed ramdisk image in bytes */
};

/* board serial number. "64 bits should be enough for everybody" */
#define ATAG_SERIAL		0x54410006

struct tag_serialnr {
	x_u32 low;
	x_u32 high;
};

/* board revision */
#define ATAG_REVISION	0x54410007

struct tag_revision {
	x_u32 rev;
};

/* initial values for vesafb-type framebuffers. see struct screen_info
 * in include/linux/tty.h
 */
#define ATAG_VIDEOLFB	0x54410008

struct tag_videolfb {
	x_u16		lfb_width;
	x_u16		lfb_height;
	x_u16		lfb_depth;
	x_u16		lfb_linelength;
	x_u32		lfb_base;
	x_u32		lfb_size;
	x_u8		red_size;
	x_u8		red_pos;
	x_u8		green_size;
	x_u8		green_pos;
	x_u8		blue_size;
	x_u8		blue_pos;
	x_u8		rsvd_size;
	x_u8		rsvd_pos;
};

/* command line: \0 terminated string */
#define ATAG_CMDLINE	0x54410009

struct tag_cmdline {
	char	cmdline[1];	/* this is the minimum size */
};

/* acorn RiscPC specific information */
#define ATAG_ACORN		0x41000101

struct tag_acorn {
	x_u32 memc_control_reg;
	x_u32 vram_pages;
	x_u8 sounddefault;
	x_u8 adfsdrives;
};

/* footbridge memory clock, see arch/arm/mach-footbridge/arch.c */
#define ATAG_MEMCLK		0x41000402

struct tag_memclk {
	x_u32 fmemclk;
};

struct tag {
	struct tag_header hdr;
	union {
		struct tag_core		core;
		struct tag_mem32	mem;
		struct tag_videotext	videotext;
		struct tag_ramdisk	ramdisk;
		struct tag_initrd	initrd;
		struct tag_serialnr	serialnr;
		struct tag_revision	revision;
		struct tag_videolfb	videolfb;
		struct tag_cmdline	cmdline;

		/*
		 * Acorn specific
		 */
		struct tag_acorn	acorn;

		/*
		 * DC21285 specific
		 */
		struct tag_memclk	memclk;
	} u;
};

struct tagtable {
	x_u32 tag;
	int (*parse)(const struct tag *);
};

#define __tag __attribute_used__ __attribute__((__section__(".taglist.init")))
#define __tagtable(tag, fn) \
static struct tagtable __tagtable_##fn __tag = { tag, fn }

#define tag_member_present(tag,member)				\
	((unsigned long)(&((struct tag *)0L)->member + 1)	\
		<= (tag)->hdr.size * 4)

#define tag_next(t)	((struct tag *)((x_u32 *)(t) + (t)->hdr.size))
#define tag_size(type)	((sizeof(struct tag_header) + sizeof(struct type)) >> 2)

#define for_each_tag(t,base)		\
	for (t = base; t->hdr.size; t = tag_next(t))

#endif /* __LINUX_H__ */
