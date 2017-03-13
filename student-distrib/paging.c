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
}

void paging_init_helper()
{
    /*
        This is where the assembly code is going to go
    */
}
