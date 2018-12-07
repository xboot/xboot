#ifndef _FAT_COMMON_H__
#define _FAT_COMMON_H__

#include <xboot.h>

typedef s8_t	s8;
typedef u8_t	u8;

typedef s16_t	s16;
typedef u16_t	u16;

typedef s32_t	s32;
typedef u32_t	u32;

typedef s64_t	s64;
typedef u64_t	u64;

typedef bool_t	bool;

/* Important offsets */
#define FAT_BOOTSECTOR_OFFSET		0x000

/* Enumeration of possible values for Media Type field in boot sector */
enum fat_media_types {
	FAT_DOUBLE_SIDED_1_44_MB=0xF0,
	FAT_FIXED_DISK=0xF8,
	FAT_DOUBLE_SIDED_720_KB=0xF9,
	FAT_SINGLE_SIDED_320_KB=0xFA,
	FAT_DOUBLE_SIDED_640_KB=0xFB,
	FAT_SINGLE_SIDED_180_KB=0xFC,
	FAT_DOUBLE_SIDED_360_KB=0xFD,
	FAT_SINGLE_SIDED_160_KB=0xFE,
	FAT_DOUBLE_SIDED_320_KB=0xFF
};

/* Enumeration of FAT types */
enum fat_types {
	FAT_TYPE_12=12,
	FAT_TYPE_16=16,
	FAT_TYPE_32=32
};

/* Enumeration of types of cluster in FAT12 table */
enum fat12_cluster_types {
	FAT12_FREE_CLUSTER=0x000,
	FAT12_RESERVED1_CLUSTER=0x001,
	FAT12_RESERVED2_CLUSTER=0xFF0,
	FAT12_BAD_CLUSTER=0xFF7,
	FAT12_LAST_CLUSTER=0xFF8
};

/* Enumeration of types of cluster in FAT16 table */
enum fat16_cluster_types {
	FAT16_FREE_CLUSTER=0x0000,
	FAT16_RESERVED1_CLUSTER=0x0001,
	FAT16_RESERVED2_CLUSTER=0xFFF0,
	FAT16_BAD_CLUSTER=0xFFF7,
	FAT16_LAST_CLUSTER=0xFFF8
};

/* Enumeration of types of cluster in FAT32 table */
enum fat32_cluster_types {
	FAT32_FREE_CLUSTER=0x00000000,
	FAT32_RESERVED1_CLUSTER=0x00000001,
	FAT32_RESERVED2_CLUSTER=0x0FFFFFF0,
	FAT32_BAD_CLUSTER=0x0FFFFFF7,
	FAT32_LAST_CLUSTER=0x0FFFFFF8
};

/* Extended boot sector information for FAT12/FAT16 */
struct fat_bootsec_ext16 {
	u8 drive_number;
	u8 reserved;
	u8 extended_signature;
	u32 serial_number;
	u8 volume_label[11];
	u8 fs_type[8];
	u8 boot_code[448];
	u16 boot_sector_signature;
} __attribute__ ((packed));

/* Extended boot sector information for FAT32 */
struct fat_bootsec_ext32 {
	u32 sectors_per_fat;
	u16 fat_flags;
	u16 version;
	u32 root_directory_cluster;
	u16 fs_info_sector;
	u16 boot_sector_copy;
	u8 reserved1[12];
	u8 drive_number;
	u8 reserved2;
	u8 extended_signature;
	u32 serial_number;
	u8 volume_label[11];
	u8 fs_type[8];
	u8 boot_code[420];
	u16 boot_sector_signature;
} __attribute__ ((packed));

/* Boot sector information for FAT12/FAT16/FAT32 */
struct fat_bootsec {
	u8 jump[3];
	u8 oem_name[8];
	u16 bytes_per_sector;
	u8 sectors_per_cluster;
	u16 reserved_sector_count;
	u8 number_of_fat;
	u16 root_entry_count;
	u16 total_sectors_16;
	u8 media_type;
	u16 sectors_per_fat;
	u16 sectors_per_track;
	u16 number_of_heads;
	u32 hidden_sector_count;
	u32 total_sectors_32;
	union {
		struct fat_bootsec_ext16 e16;
		struct fat_bootsec_ext32 e32;
	} ext;
} __attribute__ ((packed));

/* Directory entry attributes */
#define	FAT_DIRENT_READONLY	0x01
#define	FAT_DIRENT_HIDDEN	0x02
#define	FAT_DIRENT_SYSTEM	0x04
#define	FAT_DIRENT_VOLLABLE	0x08
#define	FAT_DIRENT_SUBDIR	0x10
#define	FAT_DIRENT_ARCHIVE	0x20
#define	FAT_DIRENT_DEVICE	0x40
#define	FAT_DIRENT_UNUSED	0x80

/* Directory entry information for FAT12/FAT16/FAT32 */
struct fat_dirent {
	u8 dos_file_name[8];
	u8 dos_extension[3];
	u8 file_attributes;
	u8 reserved;
	u8 create_time_millisecs;
	u32 create_time_seconds:5;
	u32 create_time_minutes:6;
	u32 create_time_hours:5;
	u32 create_date_day:5;
	u32 create_date_month:4;
	u32 create_date_year:7;
	u32 laccess_date_day:5;
	u32 laccess_date_month:4;
	u32 laccess_date_year:7;
	u16 first_cluster_hi; /* For FAT16 this is ea_index */
	u32 lmodify_time_seconds:5;
	u32 lmodify_time_minutes:6;
	u32 lmodify_time_hours:5;
	u32 lmodify_date_day:5;
	u32 lmodify_date_month:4;
	u32 lmodify_date_year:7;
	u16 first_cluster_lo; /* For FAT16 first_cluster = first_cluster_lo */
	u32 file_size;
} __attribute__ ((packed));

#define FAT_LONGNAME_ATTRIBUTE		0x0F
#define FAT_LONGNAME_LASTSEQ_MASK	0x40
#define FAT_LONGNAME_SEQNO(s)		((s) & ~0x40)
#define FAT_LONGNAME_LASTSEQ(s) 	((s) & 0x40)
#define FAT_LONGNAME_MINSEQ		1
#define FAT_LONGNAME_MAXSEQ		(VFS_MAX_NAME / 13)

/* Directory long filename information for FAT12/FAT16/FAT32 */
struct fat_longname {
	u8 seqno;
	u16 name_utf16_1[5];
	u8 file_attributes;
	u8 type;
	u8 checksum;
	u16 name_utf16_2[6];
	u16 first_cluster;
	u16 name_utf16_3[2];
} __attribute__ ((packed));

#endif
