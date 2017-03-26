#include "fs.h"

static boot_block_head_t* boot_block;
static dentry_t* dentries;
static inode_t* inodes;
static data_block_t* data_blocks;
static uint32_t max_string[MAX_DENTRY];

static void check_for_max();

void fs_init(uint8_t* fs_img){
    //pointer to beginning of fs img
    boot_block = (boot_block_head_t*)fs_img;

    //set pointers to beginning of their spots
    dentries = (dentry_t*)(fs_img + DENTRY_SIZE);
    inodes = (inode_t*)(fs_img + BLOCK_SIZE);
    data_blocks = (data_block_t*)(fs_img + BLOCK_SIZE + boot_block->num_inodes*BLOCK_SIZE);

    //figure out what dentries have strings of size 32
    check_for_max();
}

void check_for_max(){
    uint32_t i,j,max,num_de;
    uint8_t *dstr;
    num_de = boot_block->num_dir_entries;
    for(i = 0; i < num_de; i++){
        max = 0;
        dstr = (uint8_t*)(dentries[i].fname);
        for(j = 0; j < FNAME_LEN; j++){
            if(dstr[j] == '\0'){
                max_string[i] = 0;
                max = 1;
                break;
            }
        }
        if(max == 0){
            max_string[i] = 1;
        }
    }
}

int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry){
    uint32_t i,len,num_de;
    len = strlen((int8_t*)fname);
    num_de = boot_block->num_dir_entries;

    //if name is too large, truncate it
    if(len > FNAME_LEN)
        len = FNAME_LEN;

    for(i = 0; i < num_de; i++){
        //if string of this dentry is length 32
        if(max_string[i]){
            if(FNAME_LEN == len && strncmp((int8_t*)fname,dentries[i].fname,len) == 0){
                //if match, fill in dentry and return success
                strncpy(dentry->fname,dentries[i].fname,FNAME_LEN);
                dentry->ftype = dentries[i].ftype;
                dentry->inode_num = dentries[i].inode_num;
                return 0;
            }
        }
        //its safe to call strlen
        else{
            if(strlen(dentries[i].fname) == len && strncmp((int8_t*)fname,dentries[i].fname,len) == 0){
                //if match, fill in dentry and return success
                strncpy(dentry->fname,dentries[i].fname,FNAME_LEN);
                dentry->ftype = dentries[i].ftype;
                dentry->inode_num = dentries[i].inode_num;
                return 0;
            }
        }
    }
    //no dentry found with matching name
    return -1;
}

int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry){
    //if index is out of bounds
    if(index >= boot_block->num_dir_entries)
        return -1;

    strncpy(dentry->fname,dentries[index].fname,FNAME_LEN);
    dentry->ftype = dentries[index].ftype;
    dentry->inode_num = dentries[index].inode_num;

    return 0;
}

int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
    uint32_t start, db_num, db_idx, counter,len;
    inode_t *i_ptr;

    //invalid inode
    if(inode >= boot_block->num_inodes)
        return -1;
    //get pointer to inode
    i_ptr = &inodes[inode];
    //get length of file
    len = i_ptr->len;
    //offset is outside of range of file
    if(offset >= len)
        return 0;

    //pick the right data block with offset
    db_idx = offset/BLOCK_SIZE;
    db_num = i_ptr->db[db_idx];

    //data block DNE
    if(db_num >= boot_block->num_data_blocks)
        return -1;

    //get index to correct blocks
    start = offset % BLOCK_SIZE;

    counter = 0;
    //keep filling in until buffer is filled with length chars
    while(counter != length || counter+offset < len){
        buf[counter] = data_blocks[db_num].data[start];
        //if reach the end of the block, go to next block
        if(start == BLOCK_SIZE - 1){
            db_idx++;
            db_num = i_ptr->db[db_idx];
            //if the next data block is invalid
            if(db_num >= boot_block->num_data_blocks)
                return counter;
            //reset pointers
            start = 0;
        }
        else
            start++;
        counter++;
    }
    return counter;
}


void print_all_files(){
    uint32_t i,j;
    clear();
    resetCursor();
    for(i = 0; i < boot_block->num_dir_entries; i++){
        printf("File name: ");
        dentry_t d;
        read_dentry_by_index(i,&d);
        for(j = 0; j < FNAME_LEN; j++)
            printf("%c",d.fname[j]);
        printf("File type: ");
        printf("%d",d.ftype);
        printf(" File size: %d\n",inodes[d.inode_num].len);
    }
}

void read_file_by_name(int8_t* name){
    dentry_t d;
    uint32_t i,len;
    uint8_t buf[40000];
    clear();
    resetCursor();
    if(read_dentry_by_name((uint8_t*)name,&d) == -1){
        printf("File not found");
        return;
    }
    printf("File name: ");
    for(i = 0; i < FNAME_LEN; i++)
        printf("%c",d.fname[i]);
    printf("\n");
    if(d.ftype != 2){
        printf("Invalid filetype to print");
        return;
    }
    len = inodes[d.inode_num].len;
    read_data(d.inode_num,0,buf,len);
    terminal_write((int8_t*)buf,len);
}

void read_file_by_index(){
    dentry_t d;
    uint32_t i,len,j;
    int32_t key;
    uint8_t buf[40000];

    for(i = 0; i < boot_block->num_dir_entries; i++){
        key = get_buf_idx();
        read_dentry_by_index(i,&d);
        clear();
        resetCursor();
        len = inodes[d.inode_num].len;
        read_data(d.inode_num,0,buf,len);
        terminal_write((int8_t*)buf,len);
        printf("\n");
        printf("File name: ");
        for(j = 0; j < FNAME_LEN; j++)
            printf("%c",d.fname[j]);
        while(key == get_buf_idx());
    }
    clear();
}
