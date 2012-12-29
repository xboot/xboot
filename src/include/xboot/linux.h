#ifndef __LINUX_H__
#define __LINUX_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

/* The list ends with an ATAG_NONE node. */
#define ATAG_NONE		0x00000000

struct tag_header {
	u32_t size;
	u32_t tag;
};

/* The list must start with an ATAG_CORE node */
#define ATAG_CORE		0x54410001

struct tag_core {
	u32_t flags;		/* bit 0 = read-only */
	u32_t pagesize;
	u32_t rootdev;
};

/* it is allowed to have multiple ATAG_MEM nodes */
#define ATAG_MEM		0x54410002

struct tag_mem32 {
	u32_t size;
	u32_t start;	/* physical start address */
};

/* VGA text type displays */
#define ATAG_VIDEOTEXT	0x54410003

struct tag_videotext {
	u8_t		x;
	u8_t		y;
	u16_t		video_page;
	u8_t		video_mode;
	u8_t		video_cols;
	u16_t		video_ega_bx;
	u8_t		video_lines;
	u8_t		video_isvga;
	u16_t		video_points;
};

/* describes how the ramdisk will be used in kernel */
#define ATAG_RAMDISK	0x54410004

struct tag_ramdisk {
	u32_t flags;	/* bit 0 = load, bit 1 = prompt */
	u32_t size;	/* decompressed ramdisk size in _kilo_ bytes */
	u32_t start;	/* starting block of floppy-based RAM disk image */
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
	u32_t start;	/* physical start address */
	u32_t size;	/* size of compressed ramdisk image in bytes */
};

/* board serial number. "64 bits should be enough for everybody" */
#define ATAG_SERIAL		0x54410006

struct tag_serialnr {
	u32_t low;
	u32_t high;
};

/* board revision */
#define ATAG_REVISION	0x54410007

struct tag_revision {
	u32_t rev;
};

/* initial values for vesafb-type framebuffers. see struct screen_info
 * in include/linux/tty.h
 */
#define ATAG_VIDEOLFB	0x54410008

struct tag_videolfb {
	u16_t		lfb_width;
	u16_t		lfb_height;
	u16_t		lfb_depth;
	u16_t		lfb_linelength;
	u32_t		lfb_base;
	u32_t		lfb_size;
	u8_t		red_size;
	u8_t		red_pos;
	u8_t		green_size;
	u8_t		green_pos;
	u8_t		blue_size;
	u8_t		blue_pos;
	u8_t		rsvd_size;
	u8_t		rsvd_pos;
};

/* command line: \0 terminated string */
#define ATAG_CMDLINE	0x54410009

struct tag_cmdline {
	char	cmdline[1];	/* this is the minimum size */
};

/* acorn RiscPC specific information */
#define ATAG_ACORN		0x41000101

struct tag_acorn {
	u32_t memc_control_reg;
	u32_t vram_pages;
	u8_t sounddefault;
	u8_t adfsdrives;
};

/* footbridge memory clock, see arch/arm/mach-footbridge/arch.c */
#define ATAG_MEMCLK		0x41000402

struct tag_memclk {
	u32_t fmemclk;
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
	u32_t tag;
	int (*parse)(const struct tag *);
};

#define __tag __attribute_used__ __attribute__((__section__(".taglist.init")))
#define __tagtable(tag, fn) \
static struct tagtable __tagtable_##fn __tag = { tag, fn }

#define tag_member_present(tag,member)				\
	((unsigned long)(&((struct tag *)0L)->member + 1)	\
		<= (tag)->hdr.size * 4)

#define tag_next(t)	((struct tag *)((u32_t *)(t) + (t)->hdr.size))
#define tag_size(type)	((sizeof(struct tag_header) + sizeof(struct type)) >> 2)

#define for_each_tag(t,base)		\
	for (t = base; t->hdr.size; t = tag_next(t))

#ifdef __cplusplus
}
#endif

#endif /* __LINUX_H__ */
