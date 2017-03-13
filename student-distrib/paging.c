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

        3) find a way to fill the space after kernel memory with blank bullshit essesntially
    */

    // Tim this is where your code goes

    uint32_t entry = 0x0;
    uint32_t pt_addr = (uint32_t)&page_table;
    pt_addr /= 0x4000;
    entry = pt_addr << 11;
    entry += 3;
    page_directory[0] = entry;

    entry = 0x0;
    entry += 0x1 << 21;
    entry += 0x83;
    page_directory[1] = entry;

    int i;
    for(i = 2; i < DIRECTORY_SIZE; i++){
        page_directory[i] |= 0xFFFE;
    }

    entry = 0x0;
    entry = VIDEO/0x4000 << 11;
    entry += 3;
    page_table[0] = entry;

    for(i = 1; i < DIRECTORY_SIZE; i++){
        page_table[i] |= 0xFFFE;
    }


    enable_paging();

}

void enable_paging()
{
    /*
        This is where the assembly code is going to go
    */

    asm volatile(
                "movl page_directory, %%eax \n \
                movl %%eax, %%cr3 \n \
                movl %%cr0, %%eax \n \
                orl 0x80000001, %%eax \n \
                movl %%eax, %%cr0 \n \
                movl %%cr4, %%eax \n \
                orl 0x00000010, %%eax \n \
                movl %%eax, %%cr4"
                );
}
