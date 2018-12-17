#ifndef __FAT_H__
#define __FAT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <vfs/vfs.h>

/*
 * Important offsets
 */
#define FAT_BOOTSECTOR_OFFSET	(0x000)

/*
 * Enumeration of possible values for Media Type field in boot sector
 */
enum fat_media_type_t {
	FAT_DOUBLE_SIDED_1_44_MB	= 0xF0,
	FAT_FIXED_DISK				= 0xF8,
	FAT_DOUBLE_SIDED_720_KB		= 0xF9,
	FAT_SINGLE_SIDED_320_KB		= 0xFA,
	FAT_DOUBLE_SIDED_640_KB		= 0xFB,
	FAT_SINGLE_SIDED_180_KB		= 0xFC,
	FAT_DOUBLE_SIDED_360_KB		= 0xFD,
	FAT_SINGLE_SIDED_160_KB		= 0xFE,
	FAT_DOUBLE_SIDED_320_KB		= 0xFF,
};

/*
 * Enumeration of FAT types
 */
enum fat_type_t {
	FAT_TYPE_12					= 12,
	FAT_TYPE_16					= 16,
	FAT_TYPE_32					= 32,
};

/*
 * Enumeration of types of cluster in FAT12 table
 */
enum fat12_cluster_type_t {
	FAT12_FREE_CLUSTER			= 0x000,
	FAT12_RESERVED1_CLUSTER		= 0x001,
	FAT12_RESERVED2_CLUSTER		= 0xFF0,
	FAT12_BAD_CLUSTER			= 0xFF7,
	FAT12_LAST_CLUSTER			= 0xFF8,
};

/*
 * Enumeration of types of cluster in FAT16 table
 */
enum fat16_cluster_type_t {
	FAT16_FREE_CLUSTER			= 0x0000,
	FAT16_RESERVED1_CLUSTER		= 0x0001,
	FAT16_RESERVED2_CLUSTER		= 0xFFF0,
	FAT16_BAD_CLUSTER			= 0xFFF7,
	FAT16_LAST_CLUSTER			= 0xFFF8,
};

/*
 * Enumeration of types of cluster in FAT32 table
 */
enum fat32_cluster_type_t {
	FAT32_FREE_CLUSTER			= 0x00000000,
	FAT32_RESERVED1_CLUSTER		= 0x00000001,
	FAT32_RESERVED2_CLUSTER		= 0x0FFFFFF0,
	FAT32_BAD_CLUSTER			= 0x0FFFFFF7,
	FAT32_LAST_CLUSTER			= 0x0FFFFFF8,
};

/*
 * Extended boot sector information for FAT12/FAT16
 */
struct fat_bootsec_ext16_t {
	u8_t drive_number;
	u8_t reserved;
	u8_t extended_signature;
	u32_t serial_number;
	u8_t volume_label[11];
	u8_t fs_type[8];
	u8_t boot_code[448];
	u16_t boot_sector_signature;
} __attribute__ ((packed));

/*
 * Extended boot sector information for FAT32
 */
struct fat_bootsec_ext32_t {
	u32_t sectors_per_fat;
	u16_t fat_flags;
	u16_t version;
	u32_t root_directory_cluster;
	u16_t fs_info_sector;
	u16_t boot_sector_copy;
	u8_t reserved1[12];
	u8_t drive_number;
	u8_t reserved2;
	u8_t extended_signature;
	u32_t serial_number;
	u8_t volume_label[11];
	u8_t fs_type[8];
	u8_t boot_code[420];
	u16_t boot_sector_signature;
} __attribute__ ((packed));

/*
 * Boot sector information for FAT12 / FAT16 / FAT32
 */
struct fat_bootsec_t {
	u8_t jump[3];
	u8_t oem_name[8];
	u16_t bytes_per_sector;
	u8_t sectors_per_cluster;
	u16_t reserved_sector_count;
	u8_t number_of_fat;
	u16_t root_entry_count;
	u16_t total_sectors_16;
	u8_t media_type;
	u16_t sectors_per_fat;
	u16_t sectors_per_track;
	u16_t number_of_heads;
	u32_t hidden_sector_count;
	u32_t total_sectors_32;
	union {
		struct fat_bootsec_ext16_t e16;
		struct fat_bootsec_ext32_t e32;
	} ext;
} __attribute__ ((packed));

/*
 * Directory entry attributes
 */
#define	FAT_DIRENT_READONLY		0x01
#define	FAT_DIRENT_HIDDEN		0x02
#define	FAT_DIRENT_SYSTEM		0x04
#define	FAT_DIRENT_VOLLABLE		0x08
#define	FAT_DIRENT_SUBDIR		0x10
#define	FAT_DIRENT_ARCHIVE		0x20
#define	FAT_DIRENT_DEVICE		0x40
#define	FAT_DIRENT_UNUSED		0x80

/*
 * Directory entry information for FAT12 / FAT16 / FAT32
 */
struct fat_dirent_t {
	u8_t dos_file_name[8];
	u8_t dos_extension[3];
	u8_t file_attributes;
	u8_t reserved;
	u8_t create_time_millisecs;
	u32_t create_time_seconds:5;
	u32_t create_time_minutes:6;
	u32_t create_time_hours:5;
	u32_t create_date_day:5;
	u32_t create_date_month:4;
	u32_t create_date_year:7;
	u32_t laccess_date_day:5;
	u32_t laccess_date_month:4;
	u32_t laccess_date_year:7;
	u16_t first_cluster_hi;
	u32_t lmodify_time_seconds:5;
	u32_t lmodify_time_minutes:6;
	u32_t lmodify_time_hours:5;
	u32_t lmodify_date_day:5;
	u32_t lmodify_date_month:4;
	u32_t lmodify_date_year:7;
	u16_t first_cluster_lo;
	u32_t file_size;
} __attribute__ ((packed));

#define FAT_LONGNAME_ATTRIBUTE		(0x0F)
#define FAT_LONGNAME_LASTSEQ_MASK	(0x40)
#define FAT_LONGNAME_SEQNO(s)		((s) & ~0x40)
#define FAT_LONGNAME_LASTSEQ(s) 	((s) & 0x40)
#define FAT_LONGNAME_MINSEQ			(1)
#define FAT_LONGNAME_MAXSEQ			(VFS_MAX_NAME / 13)

/*
 * Directory long filename information for FAT12 / FAT16 / FAT32
 */
struct fat_longname_t {
	u8_t seqno;
	u16_t name_utf16_1[5];
	u8_t file_attributes;
	u8_t type;
	u8_t checksum;
	u16_t name_utf16_2[6];
	u16_t first_cluster;
	u16_t name_utf16_3[2];
} __attribute__ ((packed));

#ifdef __cplusplus
}
#endif

#endif /* __FAT_H__ */
