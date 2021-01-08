#include "my_malloc.h"
#include <stdio.h>


int main()
{

    my_malloc *p = f_allocator_init(4, 6);
    int *rp = (int*)allocate(p);
    *rp = ~0;

    int *sp = (int*)allocate(p);

    return 0;
}