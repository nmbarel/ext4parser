//
// ext4 parser!!!
//

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

int GROUP_Z_PADDING = 1024;
int BLOCK_SIZE_VALUES[] = {1024, 2048, 4096, 65536};
// typical block size is 4096, need to decide if I put it as a const or not
int BLOCK_SIZE = 4096;
int SUPERBLOCK_MAGIC_SIG = 0xEF53;
int STATE_VALUES[] = {0x0001, 0x0002, 0x0004}; //Cleanly unmounted, errors detected and 
// orphans being recovred 
int ERRORS_VALUES[] = {1, 2, 3}; // Continue, remount read-only, panic
int CREATOR_OS_VALUES[] = {0, 1, 2, 3, 4}; // Linux, Hurd, Masix, FreeBSD, Lites
bool REVISION_LEVEL; // False is original, True is v2 (dynamic inode sizes)
int FEATURE_COMPAT_VALUES[] = {0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200}; //for info on these flag values, refer to the docs !!!values not in this array will not cause the parser to crash, theyh will just be ignored
int FEATURE_INCOMPAT_VALUES[] = {0x1, 0x2, 0x4, 0x8, 0x10, 0x40, 0x80, 0x100, 0x200, 0x400, 0x1000, 0x2000, 0x4000, 0x8000, 0x10000}; // for info on these flag values, refer to the docs !!!values not in this array will cause the parser to crash
int FEATURE_RO_COMPAT_VALUES[] = {0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000}; // for info on these flag values, refer to the docs !!! values not in this array will cause the parser to mount, but not edit the file system
uint8_t DEF_HASH_VERSION_VALUES[] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5}; // default hash algorithm, valid values are in the docs
int DEFAULT_MOUNT_OPTS_VALUES[] = {0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0060, 0x0100, 0x0200, 0x0400, 0x0800}; // default mount options
int FLAGS_VALUES[] = {0x0001, 0x0002, 0x0004}; // signed dir hash, unsigned dir hash, test dev code
uint8_t ENCRYPT_ALGOS_VALUES[] = {0, 1, 2, 3}; // info in docs
short BG_FLAGS_VALUES[] = {0x1, 0x2, 0x4}; // info in docs
short I_MODE_VALUES[] = {0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000, 0x6000, 0x8000, 0xA000, 0xC000}; // info in docs
int I_FLAGS_VALUES [] = {0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000, 0x8000, 0x10000, 0x20000, 0x40000, 0x80000, 0x200000, 0x400000, 0x01000000, 0x04000000, 0x08000000, 0x10000000, 0x20000000, 0x80000000, 0x4BDFFF, 0x4B80FF}; // info in docs
uint8_t FILE_TYPE_VALUES[] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7};
uint8_t DX_ROOT_INFO_HASH_VERSION_VALUES[] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5}; // info in docs
int H_MAGIC = 0xEA020000;
uint8_t E_NAME_INDEX_VALUES[] = {0, 1, 2, 3, 4 ,6, 7, 8}; // for mapping check the docs


typedef struct ext4_sblocks {
	int s_inodes_count;
	int s_blocks_count_lo;
	int s_r_blocks_count_lo;
	int s_free_blocks_count_lo;
	int s_free_inodes_count;
	int s_first_data_block;
	int s_log_block_size;
	int s_log_cluster_size; //dependent on bigalloc, check docs for more info
	int s_blocks_per_group;
	int s_clusters_per_group; //dependent on bigalloc, check docs for more info
	int s_inodes_per_group;
	int s_mtime; 
	int s_wtime;
	short s_mnt_count;
	short s_max_mnt_count;
	short s_magic; // The magic signature, is 0xEF53
	short s_state; // valid values in global var STATE_VALUES
	short s_errors; // valid values in global var ERRORS_VALUES
	short s_minor_rev_level;
	int s_lastcheck;
	int s_checkinterval;
	int s_creator_os; // valid values in global var CREATOR_OS_VALUES
	int s_rev_level; // valid values in global var REVISION_LEVEL
	short s_def_resuid;
	short s_def_resgid;
} ext4_sblock;

typedef struct ext4_sblocks_drevs {
	ext4_sblock regular_sblock;
	int s_first_ino;
	short s_inode_size;
	short s_block_group_nr;
	int s_feature_compat; // valid values in global var FEATURE_COMPAT_VALUES
	int s_feature_incompat; // valid values in global var FEATURE_INCOMPAT_VALUES
    int s_feature_ro_compat; // valid values in global var FEATURE_RO_COMPAT
	uint8_t s_uuid[16]; // 128-bit UUID for volume
	char s_volume_name[16];
	char s_last_mounted[64]; // where file system was last mounted
	int s_algorithm_usage_bitmap; // not used in e2fsprogs/linux
	// Directory preallocation, should only happen if EXT4_FEATURE_COMPAT_DIR_PREALLOC flag is on
	uint8_t s_prealloc_blocks; // not used in e2fsprogs/linux
	uint8_t s_prealloc_dir_blocks; // not used in e2fsprogs/linux
	short s_reserved_gdt_blocks;
	// Directory preallocation
	// Journaling, should only happen if EXT4_FEATURE_COMAPT_HAS_JOURNAL flag is on
	uint8_t s_journal_uuid[16];
	int s_journal_inum;
	int s_journal_dev;
	int s_last_orphan;
    int s_hash_seed[4];
	uint8_t s_def_hash_version; // valid values in global var DEF_HASH_VERSION_VALUES	
	uint8_t s_jnl_backup_type;
	short s_desc_size; 
	int s_default_mount_opts; // valid values in global var DEFAULT_MOUNT_OPTS_VALUES
	int s_first_meta_bg;
	int s_mkfs_time;
	int s_jnl_blocks[17];
	// Journaling
	// 64 bit support, should only happen if EXT4_FEATURE_COMPAT_64BIT flag is on
	int s_blocks_count_hi;
	int s_r_blocks_count_hi;
	int s_free_blocks_count_hi;
	short s_min_extra_isize;
	short s_want_extra_isize;
	int s_flags; // valid values in global var FLAGS_VALUES
	short s_raid_stride;
	long s_mmp_interval;
	int s_raid_stripe_width;
	uint8_t s_log_groups_per_flex;
	uint8_t s_checksum_type; // not used in e2fsprogs/linux
	short s_reserved_pad;
	long s_kbytes_written;
	int s_snapshot_inum; // not used in e2fsprogs/linux
	int s_snapshot_id; // not used in e2fsprogs/linux
	long s_snapshot_r_blocks_count; // not used in e2fsprogs/linux
	int s_snapshot_list; // not used in e2fsprogs/linux
	int s_error_count;
	int s_first_error_time;
	int s_first_error_ino;
	long s_first_error_block;
	uint8_t s_first_error_func[32];
	int s_first_error_line;
	int s_last_error_time;
	int s_last_error_ino;
	int s_last_error_line;
	long s_last_error_block;
	uint8_t s_last_error_funcp[32];
	uint8_t s_mount_opts[64];
	int s_usr_quota_inum;
	int s_grp_quota_inum;
	int s_overhead_blocks; // always zero, calculated dynamically by the kernel
	int s_backup_bgs[2];
	uint8_t s_encrypt_algos[4]; // valid values in global var ENCRYPT_ALGOS_VALUES
	uint8_t s_encrypt_pw_salt[16];
	int s_lpf_ino;
	int s_prj_quota_inum;
	int s_checksum_seed;
	int s_reserved[98];
	int s_checksum;

} ext4_sblock_drev;


typedef struct ext4_group_descs_32bit{
	int bg_block_bitmap_lo;
	int bg_inode_bitmap_lo;
	int bg_inode_table_lo;
	short bg_free_blocks_count_lo;
	short bg_free_inodes_count_lo;
	short bg_used_dirs_count_lo;
	short bg_users_dirs_count_lo;
	short bg_flags; // valid values are in global var BG_FLAGS_VALUES
	int bg_exclude_bitmap_lo;
	short bg_block_bitmap_csum_lo;
	short bg_inode_bitmap_csum_lo;
	short bg_itable_unused_lo;
	short bg_checksum;

} ext4_group_desc_32bit;

typedef struct ext4_group_descs_64bit {
	ext4_group_desc_32bit regular_group_desc;
	int bg_block_bitmap_hi;
	int bg_inode_bitmap_hi;
	int bg_inode_table_hi;
	short bg_free_blocks_count_hi;
	short bg_free_inodes_count_hi;
	short bg_used_dirs_count_hi;
	short bg_itable_unused_hi;
	int bg_exclude_bitmap_hi;
	short bg_block_bitmap_csum_hi;
	short bg_inode_bitmap_csum_hi;
	uint32_t bg_reserved; // padding
} ext4_group_desc_64bit;

// structs and union defenitions for inode struct
typedef struct linux1 {
	int l_i_version;
} linux1;

typedef struct hurd1 {
	int h_i_translator;
} hurd1;

typedef struct masix1 {
	int m_i_reserved;
} masix1;

typedef struct linux2 {
	short l_i_blocks_high;
	short l_i_file_acl_high;
	short l_i_uid_high;
	short l_i_gid_high;
	short l_i_checksum_lo;
	short l_i_reserved;
} linux2;

typedef struct hurd2 {
	short h_i_reserved1;
	uint16_t h_i_mode_high;
	short h_i_uid_high;
	short h_i_gid_high;
	uint32_t h_i_author;
} hurd2;

typedef struct masix2 {
	short h_i_reserved1;
	uint16_t m_i_file_acl_high;
	uint32_t m_i_reserved2[2];
} masix2;

typedef struct ext4_extent_header {
	short eh_magic; // always 0xF30A
	short eh_entries;
	short eh_max; // 4
	short eh_depth; // between 0 (leaf node) and 5 (index nodes)
	int eh_generation;
} ext4_extent_header;

typedef struct ext4_extent_idx { //index nodes
	int ei_block;
	int ei_leaf_lo; 
	short ei_leaf_hi; 
	// the last two fields combined form a pointer to another node
	uint16_t ei_unused; //unused
} ext4_extent_idx;

typedef struct ext4_extent { //leaf nodes
	int ee_block;
	short ee_len; // if the value of this field is > 32768, then actual length is ee_len - 32768
	short ee_start_hi;
	int ee_start_lo;
} ext4_extent; 

typedef struct ext4_extent_tail {
	int eb_checksum;
} ext4_extent_tail;

// inode table is a linear array of inodes, size is at least
// sb.s_inode_size * sb.s_inodes_per_group in bytes
typedef struct ext4_inodes {
	short i_mode; // valid values in global var I_MODE_VALUES
	short i_uid;
	int i_size_lo;
	int i_atime;
	int i_ctime;
	int i_mtime;
	int i_dtime;
	short i_gid;
	short i_links_count;
	int i_blocks_lo;
	int i_flags; // valid values in global var I_FLAGS_VALUES
	//really not sure about the naming convention for the structs in these unions... also the structs in osd1 are only integers but I wanted to follow exact tags as written in docs
	union osd1 {
	linux1 linux1;
	hurd1 hurd1;
	masix1 masix1; 
	};
	// made it an unsigned char as I can't think of another way to handle a variable of exactly 60 bits
	// need to research about block addressing vs extent tree... not sure if they are supposed to both be present or only one
	// on one hand, they both fit together in 60 bytes, on the other it seems redundant (except for maybe backwards compatibility with ext2/3)
	ext4_extent_header ext4_extent_header;
	union i_extent_node {
		ext4_extent_idx ext4_extent_idx[3];
		ext4_extent ext4_extent[3];
	};
	int i_generation;
	int i_file_acl_lo;
	int i_size_high;
	int i_obso_faddr;
	union osd2 {
	linux2 linux2;
	hurd2 hurd2;
	masix2 masix2;
    };
	short i_extra_isize;
	short i_checksum_hi;
	int i_ctime_extra;
	int i_mtime_extra;
	int i_atime_extra;
	int i_crtime;
	int icrtime_extra;
	int i_version_hi;
	int i_projid;
} ext4_inode;

// directory entries structs - linear

#define EXT4_NAME_LEN 255
typedef struct ext4_dir_entry {
	int inode;
	short rec_len; // must me multiple of 4
	short name_len; // 16bit even though name_len is actually supposed to be 8bit ???
	char name[EXT4_NAME_LEN];
} ext4_dir_entry;

typedef struct ext4_dir_entry_2 {
	int inode;
	short rec_len;
	uint8_t name_len;
	uint8_t file_type; // valid values in global var FILE_TYPE_VALUES
	char name[EXT4_NAME_LEN];
} ext4_dir_entry_2;

typedef struct ext4_dir_entry_tail {
	int det_reserved_zero1; // must be zero
	short det_rec_len; // must be 12
	uint8_t det_reserved_zero2; // must be 0
	uint8_t def_reserved_ft; // must be 0xDE
	int def_checksum;
} ext4_dir_entry_tail;

// directory entries structs - hashed (only when flag 0x1000 is set in the inode)
typedef struct dx_entry {
	int hash; //hash code
	int block; // block number (within the dir file) of the next node in the htree
} dx_entry;

typedef struct dx_root {
	// dir entry for ".", meaning current dir
	int dot_inode;
	short dot_rec_len;
	uint8_t dot_name_len; // 1
	uint8_t dot_file_type;
	char dot_name[4]; // .\0\0\0
	// dir entry for "..", meaning parent dir
	int dotdot_inode;
	short dotdot_rec_len;
	uint8_t dotdot_name_len; // 2
	uint8_t dotdot_file_type;
	char dotdot_name[4]; // ..\0\0
	// the actual root node struct
	int dx_root_info_reserved_zero;
	uint8_t dx_root_info_hash_version; // valid values in global var DX_ROOT_INFO_HASH_VERSION_VALUES 
	uint8_t dx_root_info_info_length; // 0x8
	uint8_t dx_root_info_indirect_levels; // 3 or 2, depends on whether INCOMPAT_LARGEDIR flag is set or not
	uint8_t dx_root_info_unused_flags;
	short limit;
	short count; 
	int block;
	dx_entry entries[507]; // as many 8 byte entires as fits in the rest of the data block, so (data block - dx_root) / 8 -> (4096 - 40) / 8 -> 507  
	// I think I don't actually need to reserve the whole data block, need to check further
} dx_root;

typedef struct dx_node {
	int fake_inode; // zero
	short fake_rec_len;
	uint8_t name_len; // zero
	uint8_t file_type; // zero
	short limit;
	short count;
	int block;
	dx_entry entries[510]; // as many 8 byte entires as fits in the rest of the data block, so (data block - dx_node) / 8 -> (4096 - 16) / 8 -> 510
	// I think I don't actually need to reserve the whole data block, need to check further
} dx_node;

// extended attributes structs

typedef struct ext4_xattr_ibody_header {
	int h_magic; // valid value in H_MAGIC
} ext4_xattr_ibody_header;

typedef struct ext4_xattr_header {
	int h_magic; // valid value in H_MAGIC
	int h_refcount;
	int h_blocks;
	int h_hash;
	int h_checksum;
	uint32_t h_reserved[2];
} ext4_xattr_header;

typedef struct ext4_xattr_entry {
	uint8_t e_name_len;
	uint8_t e_name_index; // valid values in global var E_NAME_INDEX_VALUES, after mapping remove the key and store only the rest of the string as the name
	short e_value_offs;
	int e_value_inum;
	int e_value_size;
	int e_hash;
	char e_name[0]; // actual length is e_name_len, but you need to have constant array initiallizers in C, this value does NOT include trailing NULL
	// to mark the end of the key list, an instance of this struct is added with it's first four fields set to zero
} ext4_xattr_entry;

int main() {
	ext4_sblock test;
	ext4_sblock_drev test2;
	ext4_group_desc_64bit test3;
	int BLOCK_SIZE = BLOCK_SIZE_VALUES[2];
	printf("%d\n", BLOCK_SIZE);
	printf("%ld\n", sizeof(test2));
	printf("%ld\n", sizeof(test3));
}
