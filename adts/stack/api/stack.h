#pragma once
#include <stdint.h>
#include <stddef.h>
/*
 * Author:
 *      github: @afatom email: fa.adam@aol.com
 * Description:  
 *      Create a Generic Stack abstract data type that stores pointer
 *      to user provided elements of generic type (void*) The Stack is heap 
 *      allocated and can grow and shrink dynamicly and on demand.
 *      Its up to user to limit the max stack size (by passing TRUE in the stack create) 
 * Created: January 06 2021
 * Last Update: January 06 2021 
 * Notes: 
 */

#define STACK_TRUE  (1)
#define STACK_FALSE (0)
/*Incomplete type - will be completed in stack.c file (encapsulation and hiding) is alive in C */
typedef struct stack stack;
typedef uint8_t _bool;

typedef enum{
	STACK_UNITIALIZED_ERROR,				/**< Uninitialized vector error 	*/
	STACK_SUCCESS,
	STACK_ALLOCATION_ERROR,				/**< realloc error on grow/shrink   */
	STACK_OVERFLOW,
	STACK_UNDERFLOW,
	STACK_INVALID_DATA_INPUT,				/**< Ex: Null input 	*/
    STACK_UNKNOWN_ERROR
} eStackResult;


stack *stack_create(_bool is_limited, size_t max_stack_size);
void   stack_destroy(stack **p_stack, void (*fp_elementDestroy)(void* item));
eStackResult stack_push(stack *p_stack, const void *item);
const void *stack_top(const stack *p_stack);
const void *stack_pop(stack *p_stack);
_bool stack_empty(const stack *p_stack);
size_t stack_size(const stack *p_stack);
size_t stack_for_each(const stack *p_stack, void (*fp_stack_action)(const void *item, void *context), void *context);
