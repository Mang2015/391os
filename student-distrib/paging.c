// this is going to be the paging.c file
#include "paging.h"

void paging_init(void)
{
    /*
        1) initialize the page directory elements
            a. 4-8MB needs to be one directory with one table
            b. 0-4MB needs to be one directory with 1024 tables **double check with alan

        2) so basically here we need to initialize the 32 bits in each page table in each
           page directory

        3) find a way to fill the space after kernel memory with blank essesntially
    */

    int i;
    for(i = 0; i < DIRECTORY_SIZE; i++){
        page_directory[i] = 0x02;
    }

    uint32_t entry = (uint32_t)page_table;
    entry |= 0x03;
    page_directory[0] = entry;

    entry = 0x400000;
    entry |= 0x83;
    page_directory[1] = entry;

    for(i = 0; i < DIRECTORY_SIZE; i++){
        page_table[i] = 0x02;
    }
    entry = VIDEO;
    entry |= 3;
    page_table[VIDEO >> 12] = entry;

    enable_paging();
}

void enable_paging()
{
    asm volatile(
                "movl %0, %%eax \n \
                movl %%eax, %%cr3 \n \
                movl %%cr4, %%eax \n \
                orl $0x00000010, %%eax \n \
                movl %%eax, %%cr4 \n \
                movl %%cr0, %%eax \n \
                orl $0x80000001, %%eax \n \
                movl %%eax, %%cr0"
                :
                :"r"(page_directory)
                :"%eax"
                );
}
