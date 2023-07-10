`       ###EXTENTS###
Way back when, for every file a list was stored consisting of every individual block which made said file up, instead, to save space, ext switched to the use of extents. 
Instead of just storing every block, now file blocks were allocated in one or more continuous range of blocks, and only the first and last address of each blocks were stored, these first-last pairs are called extents.
There is a max of 4 extents per node. if the file is very big or very fragmented, instead of just storing the extents themselves the node becomes a tree, storing the location of different nodes that in turn store the location of extent. The maximum depth of the tree is 5.
You can know if it is a index node (stores locations of other nodes) or a leaf node (stores locations of extents) based on the eh_depth field.

Direct/Indirect Block Addressing is obsulete!!! ext4 uses extent trees only!
        ###DIRS###
in ext4, dirs are a flat file that map an arbitrary byte string to an inode number (hard links are mappings of different links to the same inode number).
Directory entries are found by reading the data block(s) assicuated with a directory file for the perticular directory entry desired.
There are two main ways to list files in a directory, linear and hash tree.
Linear: the default way, each directory lists its entries (other files and dirs in it) in an almost linear way (almost because dir entries are not split between FS blocks, so they might have to be broken and written in another node)
these entries just hold the inode number, name of the file and so on.
Hash tree: The hash tree is a new way to index file entries, added in ext4. it is backwards compatible and hides its new components as normal (linear) entires in order to fool older machines. It appears when a directory fills up a full data block with entries and needs more space, this is a much more efficient way to find all data.
It creates a hash tree, holding two types of nodes, the tree nodes and the leaf nodes.
Tree nodes themselves are seperated into the root node and other nodes. tree nodes are modeled to look like empty dir entries, and hold within them X number of entries, each entry is a hash of directory entry names and a block number (relative to the file) of the next node in the file tree. together they form a sort of lookup table that is sorted by hash value.
Tree nodes always fill a full data block, and if there are not enough entires the rest of the block is filled with N/A.
leaf nodes are the final step of the tree (depth == 0) and are just a sequential line of Linear dir entries.