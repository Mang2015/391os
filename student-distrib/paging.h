// this is going to be the paging.h file
#ifndef PAGING_H
#define PAGING_H

#include "x86_desc.h"
#include "lib.h"
#include "i8259"

int page_directory __attribute__((aligned (number_of_bytes)));
int page_table __attribute__((aligned (number_of_bytes)));

#endif
