//
// ext4 parser!!!
//

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

int GROUP_Z_PADDING = 1024;
int BLOCK_SIZE_VALUES[] = {1024, 2048, 4096, 65536};
// typical block size is 4096, need to decide if I put it as a const or not
//int BLOCK_SIZE = BLOCK_SIZE_VALUES[2]
int MAGIC = 0xEF53;
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
	struct ext4_sblock;
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

} ext4_sblock_drev;

int main() {
	ext4_sblock test;
	int BLOCK_SIZE = BLOCK_SIZE_VALUES[2];
	printf("%d\n", BLOCK_SIZE);
	printf("%ld", sizeof(ext4_sblocks_drev));
}
