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

    enable_paging();
}

void enable_paging()
{
    /*
        This is where the assembly code is going to go
    */

    asm volatile(
                "movl page_directory, %%eax"
                "movl %%eax, cr3"
                "movl cr0, %%eax"
                "orl 0x80000001, %%eax"
                "movl %%eax, cr0"
                "movl cr4, %%eax"
                "orl 0x00000010, %%eax"
                "movl %%eax, cr4"
                );
}
