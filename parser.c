//
// ext4 parser!!!
//

#include <stdio.h>
#include <stdbool.h>

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

struct ext4_sblock {
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

};

int main() {
	int BLOCK_SIZE = BLOCK_SIZE_VALUES[2];
	printf("%d\n", BLOCK_SIZE);
	printf("%d", sizeof(short));
}
