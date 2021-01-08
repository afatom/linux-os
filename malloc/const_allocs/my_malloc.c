#include <stdlib.h>
#include "my_malloc.h"
#define TRUE        (0x0)
#define FALSE     (0x1)

#include <stdio.h>


#define ALIGN(a)  (a)

typedef union 
{
    struct{
        union Header *ptr; /*pointer to next metadata, Header*/
        unsigned size;     /* Size of this block without the Header part*/
    } S;
    int aligner; /* force alignment of blocks */ 
}Header;

struct my_malloc{
    size_t total_chuncks_number;
    size_t aligned_chuck_size;
};

typedef struct{
    int is_free;
}header;



union u_malloc
{
    /* data */
};




my_malloc *f_allocator_init(size_t chunck_size, size_t total_chuncks)
{
    //validate inputs, maybe we want some logic on the inputs, round up , maybe working in units 
    //of virtual page size....etc
    //maybe we should set some global variable for status like errno (but we need to think about thread safety also) 
    //align sizeof(header) + chunck_size for better performance, in some machines unaligned address can 
    //cause disasters
    my_malloc *p = (my_malloc*)calloc(1, sizeof(my_malloc) + (sizeof(header) + chunck_size)*total_chuncks);
    if(p)
    {
        puts("32");
        p->aligned_chuck_size = ALIGN(chunck_size); 
        p->total_chuncks_number = total_chuncks;
    }
    return p;
}

void  f_destroy_allocator(my_malloc **p)
{
    //check p
    free(p);
    *p = NULL;
}

void  *allocate(my_malloc  *p)
{
    //check p
    size_t count = p->total_chuncks_number;
    size_t step = sizeof(header) + p->aligned_chuck_size;  
    void *rp = NULL;
    p++;
    header* hp;
    hp = (header*)(p);
    puts("1");
    
    while(count)
    {
        if(hp->is_free == TRUE)
        {
            puts("2");
            hp->is_free = FALSE;
            rp = hp+1;
        }
        else
        {
            char *tp = (char*)(hp) + step; 
            puts("3");
            hp = (header*)tp;
        }
        --count;
    }

    return rp;
}

void  release(my_malloc *p, void *prev)
{
    //check p and prev (we can make smart check on prev if he belongs to the heap
    //and prev allocated by comparing it to [buffer start, buffer end) )
    ((header*)prev - 1)->is_free = TRUE;    
}
