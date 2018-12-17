#ifndef __EXT4_H__
#define __EXT4_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <vfs/vfs.h>

/* Magic value used to identify an ext2 filesystem */
#define	EXT2_MAGIC						0xEF53

/* Amount of indirect blocks in an inode */
#define EXT2_DIRECT_BLOCKS				12

/* Bits used as offset in sector */
#define EXT2_SECTOR_BITS				9
#define EXT2_SECTOR_SIZE				512

/* Macro-instructions used to manage several block sizes */
#define EXT2_MIN_BLOCK_LOG_SIZE			10 /* 1024 */
#define EXT2_MAX_BLOCK_LOG_SIZE			16 /* 65536 */
#define EXT2_MIN_BLOCK_SIZE				(1 << EXT2_MIN_BLOCK_LOG_SIZE)
#define EXT2_MAX_BLOCK_SIZE				(1 << EXT2_MAX_BLOCK_LOG_SIZE)

/* Maximum file size (2 TB) */
#define EXT2_MAX_FILE_SIZE				0x20000000000ULL

/* The ext2 superblock */
struct ext2_sblock_t {
	u32_t total_inodes;
	u32_t total_blocks;
	u32_t reserved_blocks;
	u32_t free_blocks;
	u32_t free_inodes;
	u32_t first_data_block;
	u32_t log2_block_size;
	u32_t log2_fragment_size;
	u32_t blocks_per_group;
	u32_t fragments_per_group;
	u32_t inodes_per_group;
	u32_t mtime;
	u32_t utime;
	u16_t mnt_count;
	u16_t max_mnt_count;
	u16_t magic;
	u16_t fs_state;
	u16_t error_handling;
	u16_t minor_revision_level;
	u32_t lastcheck;
	u32_t checkinterval;
	u32_t creator_os;
	u32_t revision_level;
	u16_t uid_reserved;
	u16_t gid_reserved;
	u32_t first_inode;
	u16_t inode_size;
	u16_t block_group_number;
	u32_t feature_compatibility;
	u32_t feature_incompat;
	u32_t feature_ro_compat;
	u32_t unique_id[4];
	char volume_name[16];
	char last_mounted_on[64];
	u32_t compression_info;
	u8_t prealloc_blocks;
	u8_t prealloc_dir_blocks;
	u16_t reserved_gdt_blocks;
	u8_t journal_uuid[16];
	u32_t journal_inum;
	u32_t journal_dev;
	u32_t last_orphan;
	u32_t hash_seed[4];
	u8_t def_hash_version;
	u8_t jnl_backup_type;
	u16_t reserved_word_pad;
	u32_t default_mount_opts;
	u32_t first_meta_bg;
	u32_t mkfs_time;
	u32_t jnl_blocks[17];
} __attribute__ ((packed));

/* FS States */
#define EXT2_VALID_FS					1 /* Unmounted cleanly */
#define EXT2_ERROR_FS					2 /* Errors detected */

/* Error Handling */
#define EXT2_ERRORS_CONTINUE			1 /* continue as if nothing happened */
#define EXT2_ERRORS_RO					2 /* remount read-only */
#define EXT2_ERRORS_PANIC				3 /* cause a kernel panic */

/* Creator OS */
#define EXT2_OS_LINUX					0 /* Linux */
#define EXT2_OS_HURD					1 /* GNU HURD */
#define EXT2_OS_MASIX					2 /* MASIX */
#define EXT2_OS_FREEBSD					3 /* FreeBSD */
#define EXT2_OS_LITES					4 /* Lites */

/* Revision Level */
#define EXT2_GOOD_OLD_REV				0 /* Revision 0 */
#define EXT2_DYNAMIC_REV				1 /* Revision 1 with variable inode sizes, extended attributes, etc. */

/* Feature Compatibility */
#define EXT2_FEAT_COMPAT_DIR_PREALLOC	0x0001 /* Block pre-allocation for new directories */
#define EXT2_FEAT_COMPAT_IMAGIC_INODES	0x0002	 
#define EXT3_FEAT_COMPAT_HAS_JOURNAL	0x0004 /* An Ext3 journal exists */
#define EXT2_FEAT_COMPAT_EXT_ATTR		0x0008 /* Extended inode attributes are present */
#define EXT2_FEAT_COMPAT_RESIZE_INO		0x0010 /* Non-standard inode size used */
#define EXT2_FEAT_COMPAT_DIR_INDEX		0x0020 /* Directory indexing (HTree) */

/* Feature Incompatibility */
#define EXT2_FEAT_INCOMPAT_COMPRESSION	0x0001 /* Disk/File compression is used */
#define EXT2_FEAT_INCOMPAT_FILETYPE		0x0002
#define EXT3_FEAT_INCOMPAT_RECOVER		0x0004
#define EXT3_FEAT_INCOMPAT_JOURNAL_DEV	0x0008	 
#define EXT2_FEAT_INCOMPAT_META_BG		0x0010

/* Feature Read-Only Compatibility */
#define EXT2_FEAT_RO_COMPAT_SPARS_SUPER	0x0001 /* Sparse Superblock */
#define EXT2_FEAT_RO_COMPAT_LARGE_FILE	0x0002 /* Large file support, 64-bit file size */
#define EXT2_FEAT_RO_COMPAT_BTREE_DIR	0x0004 /* Binary tree sorted directory files */

/* Compression Algo Bitmap */
#define EXT2_LZV1_ALG					0 /* Binary value of 0x00000001 */
#define EXT2_LZRW3A_ALG					1 /* Binary value of 0x00000002 */
#define EXT2_GZIP_ALG					2 /* Binary value of 0x00000004 */
#define EXT2_BZIP2_ALG					3 /* Binary value of 0x00000008 */
#define EXT2_LZO_ALG					4 /* Binary value of 0x00000010 */

/* The ext2 blockgroup.  */
struct ext2_block_group_t {
	u32_t block_bmap_id;	/* Blocks bitmap block */
	u32_t inode_bmap_id;	/* Inodes bitmap block */
	u32_t inode_table_id;	/* Inodes table block */
	u16_t free_blocks;		/* Free blocks count */
	u16_t free_inodes;		/* Free inodes count */
	u16_t used_dir_cnt;		/* Directories count */
	u16_t bg_flags;
	u32_t bg_reserved[2];
	u16_t bg_itable_unused;	/* Unused inodes count */
	u16_t bg_checksum;		/* crc16(s_uuid+grouo_num+group_desc)*/
} __attribute__ ((packed));

/* The ext2 inode */
struct ext2_inode_t {
	u16_t mode;
	u16_t uid;
	u32_t size;
	u32_t atime;
	u32_t ctime;
	u32_t mtime;
	u32_t dtime;
	u16_t gid;
	u16_t nlinks;
	u32_t blockcnt;	/* Blocks of 512 bytes!! */
	u32_t flags;
	u32_t osd1;
	union {
		struct datablocks {
			u32_t dir_blocks[EXT2_DIRECT_BLOCKS];
			u32_t indir_block;
			u32_t double_indir_block;
			u32_t tripple_indir_block;
		} blocks;
		char symlink[60];
	} b;
	u32_t version;
	u32_t acl;
	u32_t dir_acl;
	u32_t fragment_addr;
	u32_t osd2[3];
} __attribute__ ((packed));

/* Inode modes */
#define EXT2_S_IFMASK					0xF000 /* Inode type mask */
#define EXT2_S_IFSOCK					0xC000 /* socket */
#define EXT2_S_IFLNK					0xA000 /* symbolic link */
#define EXT2_S_IFREG					0x8000 /* regular file */
#define EXT2_S_IFBLK					0x6000 /* block device */
#define EXT2_S_IFDIR					0x4000 /* directory */
#define EXT2_S_IFCHR					0x2000 /* character device */
#define EXT2_S_IFIFO					0x1000 /* fifo */
/* -- process execution user/group override -- */
#define EXT2_S_IUID						0x0800 /* Set process User ID */
#define EXT2_S_IGID						0x0400 /* Set process Group ID */
#define EXT2_S_IVTX						0x0200 /* sticky bit */
/* -- access rights -- */
#define EXT2_S_IRUSR					0x0100 /* user read */
#define EXT2_S_IWUSR					0x0080 /* user write */
#define EXT2_S_IXUSR					0x0040 /* user execute */
#define EXT2_S_IRGRP					0x0020 /* group read */
#define EXT2_S_IWGRP					0x0010 /* group write */
#define EXT2_S_IXGRP					0x0008 /* group execute */
#define EXT2_S_IROTH					0x0004 /* others read */
#define EXT2_S_IWOTH					0x0002 /* others write */
#define EXT2_S_IXOTH					0x0001 /* others execute */

/* Inode flags */
#define EXT2_SECRM_FL					0x00000001 /* secure deletion */
#define EXT2_UNRM_FL					0x00000002 /* record for undelete */
#define EXT2_COMPR_FL					0x00000004 /* compressed file */
#define EXT2_SYNC_FL					0x00000008 /* synchronous updates */
#define EXT2_IMMUTABLE_FL				0x00000010 /* immutable file */
#define EXT2_APPEND_FL					0x00000020 /* append only */
#define EXT2_NODUMP_FL					0x00000040 /* do not dump/delete file */
#define EXT2_NOATIME_FL					0x00000080 /* do not update .i_atime */
	/* -- Reserved for compression usage -- */
#define EXT2_DIRTY_FL					0x00000100 /* Dirty (modified) */
#define EXT2_COMPRBLK_FL				0x00000200 /* compressed blocks */
#define EXT2_NOCOMPR_FL					0x00000400 /* access raw compressed data */
#define EXT2_ECOMPR_FL					0x00000800 /* compression error */
	/* -- End of compression flags -- */
#define EXT2_BTREE_FL					0x00001000 /* b-tree format directory */
#define EXT2_INDEX_FL					0x00001000 /* hash indexed directory */
#define EXT2_IMAGIC_FL					0x00002000 /* AFS directory */
#define EXT3_JOURNAL_DATA_FL			0x00004000 /* journal file data */
#define EXT2_RESERVED_FL				0x80000000 /* reserved for ext2 library */

/* The ext2 directory entry. */
struct ext2_dirent_t {
	u32_t inode;
	u16_t direntlen;
	u8_t namelen;
	u8_t filetype;
} __attribute__ ((packed));

/* Directory entry file types */
#define EXT2_FT_UNKNOWN					0 /* Unknown File Type */
#define EXT2_FT_REG_FILE				1 /* Regular File */
#define EXT2_FT_DIR						2 /* Directory File */
#define EXT2_FT_CHRDEV					3 /* Character Device */
#define EXT2_FT_BLKDEV					4 /* Block Device */
#define EXT2_FT_FIFO					5 /* Buffer File */
#define EXT2_FT_SOCK					6 /* Socket File */
#define EXT2_FT_SYMLINK					7 /* Symbolic Link */

#ifdef __cplusplus
}
#endif

#endif /* __EXT4_H__ */
