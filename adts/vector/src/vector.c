#include "vector.h"
#include <stdio.h> /*printf*/
#include <stdlib.h>

/*******************************************************************************/
#define VECTOR_VALID 0xBEEFBEEF
#define VECTOR_DESTROYED 0xDEADBEEF
#define INCREASE_MEMORY 1
#define REDUCE_MEMORY -1
#define TRUE 1
#define FALSE 0
#define EMPTY 0
#define FUNCTION_NULL 0x0
/*******************************************************************************/
struct Vector
{
    size_t      m_magicNumber;
    size_t      m_initialCapacity;      /*original allocated memory for items)*/
    size_t      m_updatedCapacity;           /*recent/actual allocated memory for items) */
	size_t      m_nItems;     	  /* recent/actual number of items */
	size_t      m_blockSize;	  /* the chunk size to be allocated when no memory*/
	void**      m_items;			/*Data Array of items */
};
/*******************************************************************************/
static int IsOverFlow(Vector *_vector);
static int IsReallocNeeded(Vector *_vector);
static VectorResult ReallocMitemsArray(Vector *_vector, int _operation);
static int IsUnderFlow(Vector *_vector);
static int IsReduceNeeded(Vector *_vector);

/*******************************************************************************/

Vector *VectorCreate(size_t _initialCapacity, size_t _blockSize)
{
    Vector* vector;
    /*check inputs*/
    if (_initialCapacity == 0 && _blockSize == 0)
    {
        return NULL;
    }
    /*check malloc*/
    vector = (Vector *)malloc(sizeof(Vector));

    if (NULL == vector)
    {
        return NULL;
    }
    /*allocating mem for mitems arr*/
    vector->m_items = (void **)malloc((_initialCapacity + 1) * sizeof(void *));
    if (NULL == vector->m_items)
    {
        free (vector);
        return NULL;
    }
    /*Vector structure members itializing */
    vector->m_initialCapacity = _initialCapacity+1;
    vector->m_updatedCapacity = _initialCapacity+1;
    vector->m_nItems = 0;
    vector->m_blockSize = _blockSize;
    vector->m_magicNumber = VECTOR_VALID;
   
    return vector;
}
/*******************************************************************************/
void VectorDestroy(Vector** _vector, void (*_elementDestroy)(void* _item))
{
    size_t i;
    /*check params*/
    if (NULL == *_vector || (*_vector)->m_magicNumber != VECTOR_VALID)
    {
        return;
    }
    (*_vector)->m_magicNumber = VECTOR_DESTROYED;

    if(_elementDestroy != NULL)
    {
        /*Activate _elementDestroy function*/
        for (i = 1; i <= (*_vector)->m_nItems; ++i)
        {
            _elementDestroy((*_vector)->m_items[i]);
        }    
    }
    /**************************/
    free((*_vector)->m_items);
    free(*_vector);
    (*_vector) = NULL;
    return;
}
/*******************************************************************************/
VectorResult VectorAppend(Vector *_vector, void *_item)
{
    VectorResult reallocationResult;
    /********************************************/
    if (NULL == _vector || _vector->m_magicNumber != VECTOR_VALID)
    {
        return VECTOR_UNITIALIZED_ERROR;
    }

    if(NULL == _item)
    {
        return VECTOR_INVALID_DATA_INPUT;
    }
    /********************************************/
    
    /*check if vector in overflow*/
    if (IsOverFlow(_vector)== TRUE)
    {
        return VECTOR_OVERFLOW;
    }
    
    /*check if resize vector is needed*/
    if (IsReallocNeeded(_vector)==TRUE)
    {
        reallocationResult = ReallocMitemsArray(_vector, INCREASE_MEMORY);
        if (reallocationResult != VECTOR_SUCCESS)
        {
            return reallocationResult;
        }
    }
    
    _vector->m_items[++_vector->m_nItems] = _item;
    return VECTOR_SUCCESS;
}

/*******************************************************************************/
VectorResult VectorRemove(Vector *_vector, void **_pValue)
{
    if (NULL == _vector || _vector->m_magicNumber != VECTOR_VALID)
    {
        return VECTOR_UNITIALIZED_ERROR;
    }
    /*check underflow*/
    if (IsUnderFlow(_vector) == TRUE)
    {
        return VECTOR_UNDERFLOW;
    }
    /*delete*/
    *(_pValue) =_vector->m_items[_vector->m_nItems];
    --_vector->m_nItems;
    /*check if realloc needed*/
    if (IsReduceNeeded(_vector) == TRUE)
    {
        /*reduce array: call realloc function*/
        return ReallocMitemsArray(_vector, REDUCE_MEMORY);
    }
    return VECTOR_SUCCESS;
}
/*******************************************************************************/
size_t VectorSize(const Vector *_vector)
{
    if (NULL == _vector || _vector->m_magicNumber != VECTOR_VALID || _vector->m_nItems ==0 )
    {
        return EMPTY;
    }
    return _vector->m_nItems;
}
/*******************************************************************************/
size_t VectorCapacity(const Vector *_vector)
{
    if (NULL == _vector || _vector->m_magicNumber != VECTOR_VALID)
    {
        return EMPTY;
    }
    return _vector->m_updatedCapacity;
    /*return _vector->m_updatedCapacity + _vector->m_blockSize - _vector->m_initialCapacity;*/
    /**/
}
/*******************************************************************************/
VectorResult VectorGet(const Vector *_vector, size_t _index, void **_pValue)
{
    if (NULL == _vector || _vector->m_magicNumber != VECTOR_VALID)
    {
        return VECTOR_UNITIALIZED_ERROR;
    }

    if (NULL == _pValue)
    {
        return VECTOR_INVALID_DATA_INPUT;
    }

    if (_index > _vector->m_nItems  || _index == 0)
    {
        return VECTOR_INDEX_OUT_OF_BOUNDS_ERROR;
    }
    *(_pValue) = _vector->m_items[_index];
    return VECTOR_SUCCESS;
}

VectorResult VectorSet(Vector *_vector, size_t _index, void *_value)
{
    if (NULL == _vector || _vector->m_magicNumber != VECTOR_VALID)
    {
        return VECTOR_UNITIALIZED_ERROR;
    }
    if (_index > _vector->m_nItems || _index == 0)
    {
        return VECTOR_INDEX_OUT_OF_BOUNDS_ERROR;
    }
    if (NULL == _value)
    {
        return VECTOR_INVALID_DATA_INPUT;
    }
    _vector->m_items[_index]=_value;
    return VECTOR_SUCCESS;
}

size_t VectorForEach(const Vector *_vector, VectorElementAction _action, void *_context)
{
    void* elem;
    size_t i;
    if(NULL == _vector  || FUNCTION_NULL == _action || _vector->m_magicNumber != VECTOR_VALID)
    {
        return 0;
    }
    for(i = 1; i <= VectorSize(_vector); ++i)
    {
        VectorGet(_vector, i, &elem);
        _action(elem, i, _context);
      	
    }
    return i;
}




void PrintForEach(const Vector *_vector, VectorAction _action, void *_context)
{
    void* elem;
    size_t i;
    if(NULL == _vector  || FUNCTION_NULL == _action || _vector->m_magicNumber != VECTOR_VALID)
    {
        return;
    }
    for(i = 0; i < VectorSize(_vector); ++i)
    {
        VectorGet(_vector, i+1, &elem);
        _action(elem, _context);	
    }
    return;
}


/*******************************************************************************/
/*                      Static Functions Code Block                            */
/*******************************************************************************/
static int IsOverFlow(Vector *_vector)
{
    if (_vector->m_nItems +1 == _vector->m_updatedCapacity && _vector->m_blockSize == 0)
    {
        return TRUE; 
    }
    return FALSE;
}
/*******************************************************************************/

static int IsReallocNeeded(Vector *_vector)
{
    if ((_vector->m_nItems+1 == _vector->m_updatedCapacity) && (_vector->m_blockSize != 0))
    {
        return TRUE; /*realloc needed*/
    }
    return FALSE;
}
/*******************************************************************************/

static VectorResult ReallocMitemsArray(Vector *_vector, int _operation)
{
    void** temp = NULL;
    size_t newSize = 0;
    if (NULL == _vector || _vector->m_magicNumber != VECTOR_VALID)
    {
        return VECTOR_UNITIALIZED_ERROR;
    }

    if (_operation == INCREASE_MEMORY) /*increasing mem*/
    {
        newSize = _vector->m_updatedCapacity + _vector->m_blockSize;
    }

    else
    {
        newSize = _vector->m_updatedCapacity - _vector->m_blockSize;
    }
    /*Change Here*/
    /*********************************************************************/
    temp = (void **)realloc((_vector->m_items), (newSize) * sizeof(void*));
    if (NULL == temp)
    {
        return VECTOR_ALLOCATION_ERROR;
    }
    _vector->m_items = temp;
    _vector->m_updatedCapacity = newSize;
    /**********************************************************************/
    return VECTOR_SUCCESS;
}
/*******************************************************************************/

static int IsUnderFlow(Vector *_vector)
{
    if (_vector->m_nItems == 0)
    {
        return TRUE; /*under flow*/
    }
    return FALSE;
}
/*******************************************************************************/

static int IsReduceNeeded(Vector *_vector)
{
    if ((_vector->m_nItems == (_vector->m_updatedCapacity-1 - 2 * _vector->m_blockSize)) && (_vector->m_updatedCapacity > _vector->m_initialCapacity))
    {
        return TRUE; /*reduce is needed*/
    }
    return FALSE;
}
/********************************************************************************/
