#include "utility.h"

#define DEBUG_PRINT 0

struct ext2_super_block *get_super_block()
{
	return (struct ext2_super_block *)(disk_image + EXT2_BOOT_BLOCK_SIZE);
}

struct ext2_block_group_descriptor *get_group_descriptor_table(int group_number)
{
	return (struct ext2_block_group_descriptor *)(disk_image + (get_super_block()->first_data_block + group_number) * EXT2_UNLOG(get_super_block()->log_block_size));
}

struct ext2_inode *get_inode_table()
{
	return (struct ext2_inode *)(disk_image + EXT2_UNLOG(get_super_block()->log_block_size) * get_group_descriptor_table(1)->inode_table);
}

struct ext2_inode *get_inode(int inode_number)
{
	uint32_t inodes_per_group = get_super_block()->inodes_per_group;

	uint32_t block_group_index = (inode_number - 1) / inodes_per_group; // find the block group index
	uint32_t inode_index = (inode_number - 1) % inodes_per_group;		// find position of inode in the block group

	struct ext2_block_group_descriptor *group_descriptor = &get_group_descriptor_table(1)[block_group_index];
	unsigned char *inode_table = (disk_image + EXT2_UNLOG(get_super_block()->log_block_size) * group_descriptor->inode_table);
	return (struct ext2_inode *)(inode_table + inode_index * get_super_block()->inode_size);
}

unsigned char *get_block_bitmap()
{
	return (unsigned char *)(disk_image + EXT2_UNLOG(get_super_block()->log_block_size) * get_group_descriptor_table(1)->block_bitmap);
}

unsigned char *get_inode_bitmap()
{
	return (unsigned char *)(disk_image + EXT2_UNLOG(get_super_block()->log_block_size) * get_group_descriptor_table(1)->inode_bitmap);
}

uint8_t *parse_identifier(int argc, char *argv[])
{
	size_t identifier_length = argc - 2;
	uint8_t *identifier = new uint8_t[identifier_length];
	for (size_t i = 0; i < identifier_length; i++)
	{
		unsigned int temp;
		sscanf(argv[2U + i], "%x", &temp);
		identifier[i] = (uint8_t)temp;
	}
	return identifier;
}

void print_block_bitmap(ext2_super_block *super_block)
{
	// print block bitmap in bit format
	unsigned char *block_bitmap = get_block_bitmap();
	uint32_t block_size = EXT2_UNLOG(super_block->log_block_size);

	// print bitmap in binary format
	for (uint32_t i = 0; i < block_size; i++)
	{
		printf("block bitmap %d  ", block_bitmap[i]);
		for (int j = 0; j < 8; j++)
		{
			printf("%d", (block_bitmap[i] >> j) & 1);
		}
		printf("\n");
	}
}
void print_inode_bitmap(ext2_super_block *super_block)
{
	// print inode bitmap in bit format
	unsigned char *inode_bitmap = get_inode_bitmap();
	uint32_t block_size = EXT2_UNLOG(super_block->log_block_size);

	// print inode in binary format
	for (uint32_t i = 0; i < block_size; i++)
	{
		printf("inode bitmap  %d  ", inode_bitmap[i]);
		for (int j = 0; j < 8; j++)
		{
			printf("%d", (inode_bitmap[i] >> j) & 1);
		}
		printf("\n");
	}
}

void print_block_bitmap(unsigned char *block_bitmap, uint32_t block_size)
{
	// print bitmap in binary format
	for (uint32_t i = 0; i < block_size; i++)
	{
		printf("block bitmap %d  ", block_bitmap[i]);
		for (int j = 0; j < 8; j++)
		{
			printf("%d", (block_bitmap[i] >> j) & 1);
		}
		printf("\n");
	}
}

void print_inode_bitmap(unsigned char *block_inode, uint32_t inode_size)
{
	// print bitmap in binary format
	for (uint32_t i = 0; i < inode_size; i++)
	{
		printf("inode bitmap %d  ", block_inode[i]);
		for (int j = 0; j < 8; j++)
		{
			printf("%d", (block_inode[i] >> j) & 1);
		}
		printf("\n");
	}
}

void print_image(struct stat *image_st)
{
	// print stat
	if (DEBUG_PRINT)
		print_stat(image_st);

	// print additional information
	if (DEBUG_PRINT)
		print_image_info();

	// print super block
	struct ext2_super_block *super_block = get_super_block();
	if (DEBUG_PRINT)
		print_super_block(super_block);

	// print block groups
	if (DEBUG_PRINT)
	{
		uint32_t num_block_groups = get_super_block()->block_count / get_super_block()->blocks_per_group;

		uint32_t num_of_free_blocks = 0;
		uint32_t num_of_free_inodes = 0;

		for (uint32_t i = 0; i < num_block_groups; i++)
		{
			struct ext2_block_group_descriptor *group_descriptor = &get_group_descriptor_table(1)[i]; // get i-th block group
			printf("Block Group %d\n", i);
			print_group_descriptor(group_descriptor);
			unsigned char *block_bitmap = (unsigned char *)(disk_image + EXT2_UNLOG(super_block->log_block_size) * group_descriptor->block_bitmap);
			unsigned char *inode_bitmap = (unsigned char *)(disk_image + EXT2_UNLOG(super_block->log_block_size) * group_descriptor->inode_bitmap);

			// print block bitmap
			print_block_bitmap(block_bitmap, super_block->blocks_per_group / 8);
			print_inode_bitmap(inode_bitmap, super_block->inodes_per_group / 8);

			// count number of free blocks and inodes
			num_of_free_blocks += group_descriptor->free_block_count;
			num_of_free_inodes += group_descriptor->free_inode_count;
		}

		printf("Number of free blocks: %d\n", num_of_free_blocks);
		printf("Number of free inodes: %d\n", num_of_free_inodes);
	}

	// print root dir
	struct ext2_inode *inode = get_inode(EXT2_ROOT_INODE);
	// print_inode(inode, EXT2_ROOT_INODE);
	// printf("inode size: %d\n", inode->size);
	printf("- root/\n");

	print_dir(super_block, inode, 2);
}

void print_image_info()
{
	printf("###############################\n");
	printf("Block Size %ld \n", EXT2_UNLOG(get_super_block()->log_block_size));
	printf("Block per group %d \n", get_super_block()->blocks_per_group);
	printf("Block Numbers %d \n", get_super_block()->block_count / get_super_block()->blocks_per_group);
	printf("Inode per group %d \n", get_super_block()->inodes_per_group);
	printf("Inode Size %d \n", get_super_block()->inode_size);
	printf("###############################\n");
	printf("\n");
}

void print_dir(ext2_super_block *super_block, ext2_inode *inode, int depth)
{
	// start print from direct blocks
	print_direct_blocks(super_block, inode, depth);

	// single indirects
	if (inode->single_indirect != 0)
	{
		print_indirect_blocks(super_block, inode->single_indirect, depth);
	}

	// double indirects
	if (inode->double_indirect != 0)
	{
		print_double_indirect_blocks(super_block, inode->double_indirect, depth);
	}

	// triple indirects
	if (inode->triple_indirect != 0)
	{
		print_triple_indirect_blocks(super_block, inode->triple_indirect, depth);
	}
}

void print_direct_blocks(ext2_super_block *super_block, ext2_inode *inode, int depth)
{
	for (int d = 0; d < EXT2_NUM_DIRECT_BLOCKS; d++)
	{
		// printf("direct block %d: %d\n", d, inode->direct_blocks[d]);
		struct ext2_dir_entry *dir_entry = (struct ext2_dir_entry *)(disk_image + EXT2_UNLOG(super_block->log_block_size) * inode->direct_blocks[d]);

		// printf("directory name %s \n", dir_entry->name);

		if (dir_entry->inode != 0)
		{
			long unsigned i = 0;

			if (inode->mode & EXT2_I_DTYPE)
			{
				while (i < EXT2_UNLOG(super_block->log_block_size))
				{
					struct ext2_inode *inside_inode = get_inode(dir_entry->inode);
					if (inside_inode->mode & EXT2_I_FTYPE) // check if it is a file
					{
						for (int j = 0; j < depth; j++)
							printf("-");

						print_dir_entry_name(dir_entry); // print file name properly without extra characters
					}
					else if (inside_inode->mode & EXT2_I_DTYPE) // check if it is a directory
					{
						// continue if the directory name . and ..
						if (strcmp(dir_entry->name, ".") == 0 || strcmp(dir_entry->name, "..") == 0)
						{
							i += dir_entry->length;
							dir_entry = (struct ext2_dir_entry *)((unsigned char *)dir_entry + dir_entry->length);
							continue;
						}
						for (int j = 0; j < depth; j++)
							printf("-");

						print_dir_entry_name(dir_entry);

						print_dir(super_block, inside_inode, depth + 1);
					}

					i += dir_entry->length;
					dir_entry = (struct ext2_dir_entry *)((unsigned char *)dir_entry + dir_entry->length);
				}
			}
		}
	}
}

void print_indirect_blocks(ext2_super_block *super_block, uint32_t block, int depth)
{
	// single indirects
	int num_indirects = EXT2_UNLOG(super_block->log_block_size) / sizeof(uint32_t); // number of single indirects
	// printf("inode single indirect %d\n", inode->single_indirect);
	uint32_t *indirect_block = (uint32_t *)(disk_image + EXT2_UNLOG(super_block->log_block_size) * block);

	for (int i = 0; i < num_indirects; i++)
	{
		if (indirect_block[i] != 0)
		{
			// printf("indirect block %d: %d\n", i, indirect_block[i]);
			struct ext2_dir_entry *dir_entry = (struct ext2_dir_entry *)(disk_image + EXT2_UNLOG(super_block->log_block_size) * indirect_block[i]);

			if (dir_entry->inode != 0)
			{
				long unsigned j = 0;
				while (j < EXT2_UNLOG(super_block->log_block_size))
				{
					struct ext2_inode *inside_inode = get_inode(dir_entry->inode);
					if (inside_inode->mode & EXT2_I_FTYPE) // check if it is a file
					{
						for (int p = 0; p < depth; p++)
							printf("-");

						print_dir_entry_name(dir_entry);
					}
					else if (inside_inode->mode & EXT2_I_DTYPE) // check if it is a directory
					{
						// continue if the directory name . and ..
						if (strcmp(dir_entry->name, ".") == 0 || strcmp(dir_entry->name, "..") == 0)
						{
							j += dir_entry->length;
							dir_entry = (struct ext2_dir_entry *)((unsigned char *)dir_entry + dir_entry->length);
							continue;
						}
						for (int p = 0; p < depth; p++)
							printf("-");

						print_dir_entry_name(dir_entry);

						print_dir(super_block, inside_inode, depth + 1);
					}

					j += dir_entry->length;
					dir_entry = (struct ext2_dir_entry *)((unsigned char *)dir_entry + dir_entry->length);
				}
			}
		}
	}
}

void print_double_indirect_blocks(ext2_super_block *super_block, uint32_t block, int depth)
{
	uint32_t *double_indirect_block = (uint32_t *)(disk_image + EXT2_UNLOG(super_block->log_block_size) * block);
	int num_indirects = EXT2_UNLOG(super_block->log_block_size) / sizeof(uint32_t); // number of single indirects

	for (int d = 0; d < num_indirects; d++)
	{
		if (double_indirect_block[d] != 0)
			print_indirect_blocks(super_block, double_indirect_block[d], depth);
	}
}

void print_triple_indirect_blocks(ext2_super_block *super_block, uint32_t block, int depth)
{
	uint32_t *triple_indirect_block = (uint32_t *)(disk_image + EXT2_UNLOG(super_block->log_block_size) * block);
	int num_indirects = EXT2_UNLOG(super_block->log_block_size) / sizeof(uint32_t); // number of single indirects

	for (int d = 0; d < num_indirects; d++)
	{
		if (triple_indirect_block[d] != 0)
			print_double_indirect_blocks(super_block, triple_indirect_block[d], depth);
	}
}

void print_dir_entry_name(ext2_dir_entry *dir_entry)
{
	printf(" ");
	for (int r = 0; r < dir_entry->name_length; r++)
	{
		printf("%c", dir_entry->name[r]);
	}
	if (dir_entry->file_type == 2)
	{
		printf("/");
	}
	printf("\n");
}

void recover_inode_bitmaps()
{

	struct ext2_super_block *super_block = get_super_block();
	// print_super_block(super_block);
	uint32_t num_block_groups = super_block->block_count / super_block->blocks_per_group;

	uint32_t total_num_of_free_inodes = 0;
	// uint32_t current_num_of_free_inodes = 0;

	// traverse each block group
	for (uint32_t i = 0; i < num_block_groups; i++)
	{
		struct ext2_block_group_descriptor *group_descriptor = &get_group_descriptor_table(1)[i]; // get i-th block group

		unsigned char *inode_bitmap = (unsigned char *)(disk_image + EXT2_UNLOG(super_block->log_block_size) * group_descriptor->inode_bitmap);

		// count number of free blocks and inodes
		total_num_of_free_inodes += group_descriptor->free_inode_count;

		// get inode table
		unsigned char *inode_table = (unsigned char *)(disk_image + EXT2_UNLOG(super_block->log_block_size) * group_descriptor->inode_table);

		// traverse inode table
		for (uint32_t j = 0; j < super_block->inodes_per_group; j++)
		{
			// checking for if inode is used return 1 if used
			if ((inode_bitmap[j / 8] >> (j % 8)) & 0x1)
			{
				// inode is used

				// struct ext2_inode *inode = (struct ext2_inode *)(inode_table + j * super_block->inode_size);
				// printf("inode size %d\n", inode->size);
				// print_inode(inode, j+1);
				// printf("\n");
			}
			else
			{
				// inode is free
				// check if node is full

				// printf("free inode  %d\n", j+1);
				struct ext2_inode *inode = (struct ext2_inode *)(inode_table + j * super_block->inode_size);

				if (inode->size != 0 || inode->mode != 0 || inode->creation_time != 0 || inode->access_time != 0 || inode->modification_time != 0 || inode->deletion_time != 0 || inode->block_count_512 != 0)
				{
					if (inode->link_count > 0)
					{
						inode_bitmap[j / 8] |= 1 << (j % 8);
					}

					if (i == 0 && j < super_block->first_inode)
					{
						inode_bitmap[j / 8] |= 1 << (j % 8);
					}
				}
				else
				{
					if (i == 0 && j < super_block->first_inode)
					{
						inode_bitmap[j / 8] |= 1 << (j % 8);
					}
				}
			}
		}
	}
}

void recover_block_bitmaps()
{
	struct ext2_super_block *super_block = get_super_block();
	// print_super_block(super_block);
	uint32_t num_block_groups = super_block->block_count / super_block->blocks_per_group;

	// uint32_t total_num_of_free_blocks = 0;

	// traverse each block group
	for (uint32_t i = 0; i < num_block_groups; i++)
	{
		struct ext2_block_group_descriptor *group_descriptor = &get_group_descriptor_table(1)[i]; // get i-th block group

		unsigned char *block_bitmap = (unsigned char *)(disk_image + EXT2_UNLOG(super_block->log_block_size) * group_descriptor->block_bitmap);
		unsigned char *inode_bitmap = (unsigned char *)(disk_image + EXT2_UNLOG(super_block->log_block_size) * group_descriptor->inode_bitmap);

		unsigned char *inode_table = (unsigned char *)(disk_image + EXT2_UNLOG(super_block->log_block_size) * group_descriptor->inode_table);

		// access data block
		uint32_t data_block_index = group_descriptor->inode_table + (super_block->inodes_per_group * super_block->inode_size) / EXT2_UNLOG(super_block->log_block_size);
		uint32_t loop_range = super_block->blocks_per_group * (i + 1);
		for (uint32_t k = data_block_index; k < loop_range; k++)
		{
			struct ext2_dir_entry *dir_entry = (struct ext2_dir_entry *)(disk_image + EXT2_UNLOG(super_block->log_block_size) * k);

			uint32_t k_index = k % super_block->blocks_per_group;
			if (k_index == (data_block_index % super_block->blocks_per_group))
			{
				for (uint32_t w = 0; w < (data_block_index % super_block->blocks_per_group); w++)
				{
					block_bitmap[w / 8] |= (1 << (w % 8));
				}
			}

			if (dir_entry->inode != 0)
			{
				block_bitmap[k_index / 8] |= (1 << (k_index % 8));
			}
		}
	}
}
