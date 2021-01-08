#pragma once
#include <stddef.h>

typedef struct my_malloc my_malloc;


my_malloc *f_allocator_init(size_t chunck_size, size_t total_chuncks);
void  f_destroy_allocator(my_malloc **p);
void  *allocate(my_malloc  *p);
void  release(my_malloc *p, void *prev);

