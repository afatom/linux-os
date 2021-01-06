#include <stdlib.h>
#include "stack.h"
#include "vector.h"

#define FIXED_SIZE_VECTOR        (0)
#define GROWTH_RATE(_cap_)       ( (20/100)*(_cap_) )

typedef Vector vector;

struct stack
{
    vector *p_vec;
};


static inline eStackResult convert_enum(VectorResult vstat);

stack *stack_create(_bool is_limited, size_t stack_size)
{
    stack *sp = NULL;
    sp = (stack*)malloc(sizeof(struct stack));
    if(!sp)
    {
        if(is_limited == STACK_TRUE)
        {
            if(stack_size)
            {
                sp->p_vec = VectorCreate(stack_size, FIXED_SIZE_VECTOR);
            }
            else
            {
                free(sp);
                sp = NULL;
            }
        }
        else
        {
            sp->p_vec = VectorCreate(stack_size, GROWTH_RATE(stack_size));
        }
    }
    
    return sp;
}

eStackResult stack_push(stack *p_stack, const void *item)
{
    eStackResult status;
    if(p_stack)
    {
        if(item)
        {
            VectorResult vstat = VectorAppend(p_stack->p_vec, (void*)item);
            status = convert_enum(vstat);
        }
        else
        {
            status = STACK_INVALID_DATA_INPUT;
        }
    }
    else
    {
        status = STACK_UNITIALIZED_ERROR; 
    }    
    
    return status;
}



static inline eStackResult convert_enum(VectorResult vstat)
{
    eStackResult estack_stat;
    switch(vstat)
    {
        case VECTOR_SUCCESS:
        estack_stat = STACK_SUCCESS;
        break;

        case VECTOR_UNITIALIZED_ERROR:
        estack_stat = STACK_UNITIALIZED_ERROR;
        break;
        
        case VECTOR_ALLOCATION_ERROR:
        estack_stat = STACK_ALLOCATION_ERROR;
        break;

        case VECTOR_OVERFLOW:
        estack_stat = STACK_OVERFLOW;
        break;

        default:
        estack_stat = STACK_UNKNOWN_ERROR;
        break;
    }

    return estack_stat;
}
