#ifndef FS_H
#define FS_H

#include "lib.h"
#include "terminal.h"
#include "keyboard.h"

#define FNAME_LEN 32
#define RSRV_BYTES 24
#define RSRV_BOOT 52
#define BLOCK_SIZE 4096
#define DENTRY_SIZE 64
#define MAX_DENTRY 63
#define FILESYS "/filesys_img"
#define LEN_COMP 12
#define INT_SIZE 4
#define MAX_DB 1023
#define MAX_DATA BLOCK_SIZE

typedef struct dentry{
    int8_t fname[FNAME_LEN];
    uint32_t ftype;
    uint32_t inode_num;
    uint8_t reserved[RSRV_BYTES];
}dentry_t;

typedef struct inode{
    uint32_t len;
    uint32_t db[MAX_DB];
}inode_t;

typedef struct db{
    int8_t data[MAX_DATA];
}data_block_t;

typedef struct bb{
    uint32_t num_dir_entries;
    uint32_t num_inodes;
    uint32_t num_data_blocks;
    uint8_t reserved[RSRV_BOOT];
}boot_block_head_t;


void fs_init(uint8_t* fs_img);
void print_all_files();
void read_file_by_name(int8_t* name);
void read_file_by_index();
uint32_t get_length(uint32_t inode);

uint32_t fopen(const int8_t* fname);
uint32_t fread(uint32_t inode, uint32_t offset, int8_t* buf, int32_t nbytes);
uint32_t fwrite(uint32_t inode, uint32_t offset, const int8_t* buf, int32_t nbytes);
uint32_t fclose();
uint32_t f_driver(uint32_t cmd, uint32_t fd, void* buf, int32_t nbytes);


uint32_t dopen();
uint32_t dread(const int8_t* fname, dentry_t* buf);
uint32_t dread_idx(int32_t idx, dentry_t* buf);
uint32_t dwrite(const int8_t* buf);
uint32_t dclose();
uint32_t d_driver(uint32_t cmd, uint32_t fd, void* buf, int32_t nbytes);

#endif
