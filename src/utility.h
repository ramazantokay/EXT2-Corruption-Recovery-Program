#ifndef UTUILITY_H
#define UTUILITY_H

#include "ext2fs.h"
#include <stdio.h>
#include <string>
#include <cstring>
#include "ext2fs_print.h"

extern unsigned char * disk_image;


// Helper functions

struct ext2_super_block* get_super_block();
struct ext2_block_group_descriptor* get_group_descriptor_table(int group_number);

struct ext2_inode* get_inode_table();
struct ext2_inode* get_inode(int index);

unsigned char* get_block_bitmap();
unsigned char* get_inode_bitmap();


uint8_t* parse_identifier(int argc, char* argv[]);

// debug purposes
void print_block_bitmap(ext2_super_block *super_block);
void print_inode_bitmap(ext2_super_block *super_block);
void print_image(struct stat *image_st);

void print_image_info();

void print_dir(ext2_super_block *super_block, ext2_inode *inode, int depth = 1);


void print_direct_blocks(ext2_super_block *super_block, ext2_inode *inode, int depth);
void print_indirect_blocks(ext2_super_block *super_block, uint32_t block, int depth);
void print_double_indirect_blocks(ext2_super_block *super_block, uint32_t block, int depth);
void print_triple_indirect_blocks(ext2_super_block *super_block, uint32_t block, int depth);

void print_dir_entry_name(ext2_dir_entry *dir_entry); // print file name properly without extra characters

void recover_inode_bitmaps();
void recover_block_bitmaps();


#endif // !UTUILITY_H