// this is going to be the paging.h file
#ifndef PAGING_H
#define PAGING_H

#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"

#define DIRECTORY_SIZE 1024
#define PAGE_SIZE (DIRECTORY_SIZE * 4)

uint32_t page_directory __attribute__((aligned (PAGE_SIZE)));
uint32_t page_table __attribute__((aligned (PAGE_SIZE)));

extern void paging_init(void);
extern void paging_init_helper();

#endif
