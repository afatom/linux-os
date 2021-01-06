#include <stdio.h>
#include <stdlib.h>
#include "vector.h"
#include "prutest.h"
#include <time.h>
#include <string.h>
#include <unistd.h>

#define PERSONS_ARR_SIZE 5
#define BLOCK_SIZE 50
#define INITIAL_SIZE 600

#define ARR_HUGE_SIZE 5

#define TRUE 1
#define FALSE 0

UNIT(ssssrand)
srand((unsigned int)time(NULL));
END_UNIT

typedef struct Person{
        int age;
        int edu;
        char name[32];
    }Person; 

void PrintVectorResult(VectorResult _res)
{
    switch(_res)
    {
        case VECTOR_INVALID_DATA_INPUT:
        puts("VECTOR_INVALID_DATA_INPUT"); break;

        case VECTOR_UNITIALIZED_ERROR:
        puts("VECTOR_UNITIALIZED_ERROR"); break;

        case VECTOR_ALLOCATION_ERROR:
        puts("VECTOR_ALLOCATION_ERROR"); break;

        case VECTOR_INDEX_OUT_OF_BOUNDS_ERROR:
        puts("VECTOR_INDEX_OUT_OF_BOUNDS_ERROR"); break;

        case VECTOR_OVERFLOW:
        puts("VECTOR_OVERFLOW"); break;

        case VECTOR_SUCCESS:
        puts("VECTOR_SUCCESS"); break;

        case VECTOR_UNDERFLOW:
        puts("VECTOR_UNDERFLOW"); break;

        default: break;
    }
}

int PrintPersonInColor(void *_element, size_t _index, void *_context)
{
    printf("Person-%lu---Age---%d---Edu---%d.\n", _index, ((Person *)_element)->age, ((Person *)_element)->edu);
    return 1;

}

int PrintPerson(void *_element, size_t _index, void *_context)
{
    puts("Person Details:");
    printf("age %d edu %d name %s\n", ((Person *)_element)->age, ((Person *)_element)->edu, ((Person *)_element)->name);
    printf("Found at index %lu\n", _index);
    return 1;
}

void PrintOnePerson(void *_element, void *_context)
{
    printf("age  %d  edu  %d \n", ((Person *)_element)->age, ((Person *)_element)->edu);
}

static void PersonDestroy(void *_item)
{
    if ((Person *)_item == NULL)
    {
        return;
    }
    free((Person *)_item);
    /*_item = NULL;*/
}

static void swap(int* arr, int firstindex, int secindex)
{
    int firstval, secval;
    firstval =  arr[firstindex];
    secval =  arr[secindex];
    arr[secindex] = firstval;
    arr[firstindex]= secval;
    return;
}

static void ShuffleArray(int *_arr, int _size)
{
    int i, newIndex;
    for (i = _size - 1; i >= 0; --i)
    {
        newIndex = rand() % (i + 1);
        swap(_arr, i, newIndex);
    }
}

static int CheckItemInVectorValidity(Person* _person, Vector* _vec, size_t _index)
{
    void* pval = NULL;
    VectorGet(_vec,_index,&pval);
    if(_person->age != ((Person*)pval)->age || _person != pval )
    {
        return FALSE;
    }
    return TRUE;
}

UNIT(Vector__Create__Test)
    Vector *vec = NULL;
    vec = VectorCreate(10, 4);
    ASSERT_THAT(vec != NULL);
    VectorDestroy(&vec,NULL);
    ASSERT_THAT(NULL == vec);
END_UNIT


UNIT(Vector__Create__Initial_Zero__Test)
    Vector *vec = NULL;
    vec = VectorCreate(0, 10);
    ASSERT_THAT(vec != NULL);
    VectorDestroy(&vec,NULL);
    ASSERT_THAT(NULL == vec);
END_UNIT

UNIT(Create_Normal_Inputs_Test)
    Vector *ptr = NULL;
    ptr = VectorCreate(10, 4);
    ASSERT_THAT( ptr != NULL);
    VectorDestroy(&ptr,NULL);
    ASSERT_THAT(NULL == ptr);
END_UNIT

UNIT(Create_Zero_Both_Inputs_Test)
    Vector *ptr = NULL;
    ptr = VectorCreate(0, 0);
 	ASSERT_THAT( ptr == NULL);
    VectorDestroy(&ptr,NULL);
    ASSERT_THAT(NULL == ptr);
END_UNIT

UNIT(Create_Zero_Initial_Size_Test)
    Vector *ptr = NULL;
    ptr = VectorCreate(0, 4);
 	ASSERT_THAT( ptr != NULL);
    VectorDestroy(&ptr,NULL);
    ASSERT_THAT(NULL == ptr);
END_UNIT

UNIT(Create_Zero_IncrBlock_Test)
	Vector *ptr = NULL;
    ptr = VectorCreate(4, 0);
 	ASSERT_THAT( ptr != NULL);
    VectorDestroy(&ptr,NULL);
    ASSERT_THAT(NULL == ptr);
END_UNIT

UNIT(Create_Huge_Initial_Allocation_Test)
	Vector *ptr = NULL;
    ptr = VectorCreate(123456789987456213, 5);
 	ASSERT_THAT( ptr == NULL);
    VectorDestroy(&ptr,NULL);
    ASSERT_THAT(NULL == ptr);
END_UNIT

UNIT(Vector__Destroy__Null__Fp__Test)
    Vector *vec = NULL;
    vec = VectorCreate(10, 4);
    ASSERT_THAT(vec != NULL);
    VectorDestroy(&vec, NULL);
    VectorDestroy(&vec, NULL);
    ASSERT_THAT(NULL == vec);
END_UNIT

UNIT(Vector__Append__NULL__Test)
    Vector *vec = NULL;
    VectorResult res;
    vec = VectorCreate(10, 4);
    ASSERT_THAT(vec != NULL);
    ASSERT_THAT((res = VectorAppend(vec, NULL)) == VECTOR_INVALID_DATA_INPUT);
    VectorDestroy(&vec, NULL);
    VectorDestroy(&vec, NULL);
    ASSERT_THAT(NULL == vec);
END_UNIT

UNIT(Vector__Append__NULL__Vector_Test)
    Vector *vec = NULL;
    VectorResult res;
    int item;
    int* pitem = &item;
    ASSERT_THAT(vec == NULL);
    ASSERT_THAT((res = VectorAppend(vec, (void*)pitem)) == VECTOR_UNITIALIZED_ERROR);
    ASSERT_THAT(NULL == vec);
END_UNIT

UNIT(Vector__Append__Few__Test)
    Vector *vec = NULL;
    VectorResult res;

    Person p1 = {11,11,"adham"};
    Person p2 = {22, 22, "bruce"};
    Person p3 = {33,33,"Nico"};
    Person p4 = {44, 44, "segal"}; /*Memory leak*/
    
    Person* arr[4];
    arr[0]=&p1;
    arr[1]=&p2;
    arr[2]=&p3;
    arr[3]=&p4;
    
    vec = VectorCreate(10, 4);
    ASSERT_THAT(vec != NULL);
    
    ASSERT_THAT((res = VectorAppend(vec, (arr[0]))) == VECTOR_SUCCESS);
    
    ASSERT_THAT((res = VectorAppend(vec, (arr[1]))) == VECTOR_SUCCESS);
    
    ASSERT_THAT((res = VectorAppend(vec, (arr[2]))) == VECTOR_SUCCESS);
    
    ASSERT_THAT((res = VectorAppend(vec, (arr[3]))) == VECTOR_SUCCESS);
    
    ASSERT_THAT(VectorSize(vec)== 4);

    /*VectorForEach(vec, PrintPersonInColor, NULL);
    */
    ASSERT_THAT( CheckItemInVectorValidity(arr[0], vec, 1) == TRUE);
    ASSERT_THAT( CheckItemInVectorValidity(arr[1], vec, 2) == TRUE);
    ASSERT_THAT( CheckItemInVectorValidity(arr[2], vec, 3) == TRUE);
    
    ASSERT_THAT( CheckItemInVectorValidity(arr[3], vec, 4) == TRUE);

    VectorDestroy(&vec, NULL);
    ASSERT_THAT(NULL == vec);
    
END_UNIT

UNIT(Vector__Append__Many__Test)
int i;
Vector *vec = NULL;
VectorResult res;
Person **arr = NULL;
Person *getPerson = NULL;
int shufled[ARR_HUGE_SIZE];

arr = (Person **)malloc(ARR_HUGE_SIZE * sizeof(Person *));
ASSERT_THAT(arr != NULL);

for (i = 0; i < ARR_HUGE_SIZE; ++i)
{
    arr[i] = (Person *)malloc(1 * sizeof(Person));
    shufled[i] = i;
}

ShuffleArray(shufled, ARR_HUGE_SIZE);

getPerson = (Person *)malloc(1 * sizeof(Person));

vec = VectorCreate(INITIAL_SIZE, BLOCK_SIZE);
ASSERT_THAT(vec != NULL);

for (i = 0; i < ARR_HUGE_SIZE; ++i)
{
    ASSERT_THAT((res = VectorAppend(vec, (void *)(arr[shufled[i]]))) == VECTOR_SUCCESS);
}

for (i = 1; i <= ARR_HUGE_SIZE; ++i)
{
    ASSERT_THAT((res = VectorGet(vec, (size_t)i, (void *)(&getPerson))) == VECTOR_SUCCESS);
    ASSERT_THAT(getPerson == arr[shufled[i - 1]]);
}


VectorDestroy(&vec, PersonDestroy);

free(arr);
ASSERT_THAT(NULL == vec);
END_UNIT

UNIT(Vector__Append__With__Realloc__Test)
    int i;
    Vector *vec = NULL;
    VectorResult res;
    typedef struct Person
    {
        int age;
        int edu;
        char name[32];
    } Person;
    Person** arr = NULL;
   
    arr = (Person **)malloc(PERSONS_ARR_SIZE * sizeof(Person*));
 
    ASSERT_THAT(arr != NULL);

    for(i = 0; i < PERSONS_ARR_SIZE; ++i)
    {
        arr[i] = (Person *)malloc(sizeof(Person));
    }
    vec = VectorCreate(1000, 1000);
    ASSERT_THAT(vec != NULL);
    for(i =0; i<PERSONS_ARR_SIZE; ++i)
    {
        ASSERT_THAT((res = VectorAppend(vec, (void*)(arr[i]))) == VECTOR_SUCCESS);
    }
    ASSERT_THAT(VectorSize(vec) == PERSONS_ARR_SIZE);

    for (i = 0; i < PERSONS_ARR_SIZE; ++i)
    {
        arr[i]=NULL;
        free(arr[i]);
    }
    
    VectorDestroy(&vec, NULL);
    free(arr);
    ASSERT_THAT(NULL == vec);
END_UNIT

UNIT(Vector__Append__With__Initial__Size__Zero__Test)
    int i;
    Vector *vec = NULL;
    VectorResult res;
    Person** arr = NULL;

    arr = (Person **)malloc(PERSONS_ARR_SIZE * sizeof(Person*));

    ASSERT_THAT(arr != NULL);

    for(i = 0; i < PERSONS_ARR_SIZE; ++i)
    {
        arr[i] = (Person*)malloc(sizeof(Person));
    }
    vec = VectorCreate(0, PERSONS_ARR_SIZE);
    ASSERT_THAT(vec != NULL);
    for(i =0; i<PERSONS_ARR_SIZE; ++i)
    {
        ASSERT_THAT((res = VectorAppend(vec, (arr[i]))) == VECTOR_SUCCESS);
    }
    ASSERT_THAT(VectorSize(vec) == PERSONS_ARR_SIZE);
    
    VectorDestroy(&vec, PersonDestroy);
    

    free(arr);

    ASSERT_THAT(NULL == vec);
END_UNIT

UNIT(Vector__Append__OverFlow__Test)
    int i;
    Vector *vec = NULL;
    VectorResult res;
    typedef struct Person
    {
        int age;
        int edu;
        char name[32];
    } Person;
    Person **arr = NULL;
    
    arr = (Person **)malloc(PERSONS_ARR_SIZE * sizeof(Person *));
    
    ASSERT_THAT(arr != NULL);
    for (i = 0; i < PERSONS_ARR_SIZE; ++i)
    {
        arr[i] = (Person *)malloc(PERSONS_ARR_SIZE * sizeof(Person));
    }
    vec = VectorCreate(PERSONS_ARR_SIZE-1, 0);
    ASSERT_THAT(vec != NULL);
    for (i = 0; i <PERSONS_ARR_SIZE-1; ++i)
    {
        ASSERT_THAT((res = VectorAppend(vec, (void *)(arr[i]))) == VECTOR_SUCCESS);
    }
    ASSERT_THAT(VectorSize(vec) == PERSONS_ARR_SIZE-1);
    ASSERT_THAT((res = VectorAppend(vec, (void *)(arr[i]))) == VECTOR_OVERFLOW);

    
    for (i = 0; i < PERSONS_ARR_SIZE; ++i)
    {
        arr[i] = NULL;
        free(arr[i]);
    }

    VectorDestroy(&vec, NULL);
    free(arr);
    ASSERT_THAT(NULL == vec);
END_UNIT

UNIT(Vector__Append__Invalid__Data__In__Test)
    int i;
    Vector *vec = NULL;
    VectorResult res;
    typedef struct Person
    {
        int age;
        int edu;
        char name[32];
    } Person;
    Person **arr = NULL;
    
    arr = (Person **)malloc(PERSONS_ARR_SIZE * sizeof(Person *));
    
    ASSERT_THAT(arr != NULL);
    
    for (i = 0; i < PERSONS_ARR_SIZE; ++i)
    {
        arr[i] = (Person *)malloc(sizeof(Person));
    }
    vec = VectorCreate(PERSONS_ARR_SIZE, 0);
    ASSERT_THAT(vec != NULL);
    for (i = 0; i < PERSONS_ARR_SIZE - 1; ++i)
    {
        ASSERT_THAT((res = VectorAppend(vec, (void *)(arr[i]))) == VECTOR_SUCCESS);
    }

    ASSERT_THAT((res = VectorAppend(vec,NULL)) == VECTOR_INVALID_DATA_INPUT);

    for (i = 0; i < PERSONS_ARR_SIZE; ++i)
    {
        arr[i] = NULL;
        free(arr[i]);
    }

    VectorDestroy(&vec, NULL);
    free(arr);
    ASSERT_THAT(NULL == vec);
END_UNIT

UNIT(Vector__Destroy__With__FuncPointer__Test)
    int i;
    Vector *vec = NULL;
    VectorResult res;
    Person **arr = NULL;
   
    arr = (Person **)malloc(PERSONS_ARR_SIZE * sizeof(Person *));
    
    ASSERT_THAT(arr != NULL);
    
    for (i = 0; i < PERSONS_ARR_SIZE; ++i)
    {
        arr[i] = (Person*)malloc(1*sizeof(Person));
    }
    vec = VectorCreate(PERSONS_ARR_SIZE, BLOCK_SIZE);
    ASSERT_THAT(vec != NULL);
    for (i = 0; i < PERSONS_ARR_SIZE; ++i)
    {
        ASSERT_THAT((res=VectorAppend(vec, (void *)(arr[i]))) == VECTOR_SUCCESS);
    }

    VectorDestroy(&vec, PersonDestroy);
    
    free(arr);
    ASSERT_THAT(NULL == vec);
END_UNIT

UNIT(Vector__Remove__One__Item__Test) 
int i;
Vector *vec = NULL;
VectorResult res;
Person **arr = NULL;
Person* deletedPerson = NULL;
arr = (Person **)malloc(PERSONS_ARR_SIZE * sizeof(Person *));
ASSERT_THAT(arr != NULL);
for (i = 0; i < PERSONS_ARR_SIZE; ++i)
{
    arr[i] = (Person *)malloc(PERSONS_ARR_SIZE * sizeof(Person));
}
vec = VectorCreate(INITIAL_SIZE, BLOCK_SIZE);
ASSERT_THAT(vec != NULL);
for (i = 0; i < PERSONS_ARR_SIZE; ++i)
{
    ASSERT_THAT((res = VectorAppend(vec, (void *)(arr[i]))) == VECTOR_SUCCESS);
}

ASSERT_THAT((res = VectorRemove(vec, (void *)(&deletedPerson))) == VECTOR_SUCCESS);
/*PrintVectorResult(res);*/
ASSERT_THAT(deletedPerson !=NULL);

VectorDestroy(&vec, NULL);
free(arr);
ASSERT_THAT(NULL == vec);
END_UNIT

UNIT(Vector__Remove__EmptyVector__Test)
    int i;
    Vector *vec = NULL;
    VectorResult res;
    Person **arr = NULL;
    Person *deletedPerson = NULL;
    arr = (Person **)malloc(PERSONS_ARR_SIZE * sizeof(Person *));
    ASSERT_THAT(arr != NULL);
    for (i = 0; i < PERSONS_ARR_SIZE; ++i)
    {
        arr[i] = (Person *)malloc(PERSONS_ARR_SIZE * sizeof(Person));
    }
    vec = VectorCreate(INITIAL_SIZE, BLOCK_SIZE);
    ASSERT_THAT(vec != NULL);
    for (i = 0; i < PERSONS_ARR_SIZE; ++i)
    {
        ASSERT_THAT((res = VectorAppend(vec, (void *)(arr[i]))) == VECTOR_SUCCESS);
    }

    for (i = 0; i < PERSONS_ARR_SIZE; ++i)
    {
        ASSERT_THAT((res = VectorRemove(vec, (void *)(&deletedPerson))) == VECTOR_SUCCESS);
        ASSERT_THAT(deletedPerson != NULL);
    }
    ASSERT_THAT((res = VectorRemove(vec, (void *)(&deletedPerson))) == VECTOR_UNDERFLOW);

    VectorDestroy(&vec, NULL);
    free(arr);
    ASSERT_THAT(NULL == vec);
END_UNIT

UNIT(Vector__Remove__Many__With__Realloc__Test)
int i;
Vector *vec = NULL;
VectorResult res;
Person **arr = NULL;
Person *deletedPerson = NULL;
arr = (Person **)malloc(PERSONS_ARR_SIZE * sizeof(Person *));
ASSERT_THAT(arr != NULL);
for (i = 0; i < PERSONS_ARR_SIZE; ++i)
{
    arr[i] = (Person *)malloc(PERSONS_ARR_SIZE * sizeof(Person));
}
vec = VectorCreate(INITIAL_SIZE, BLOCK_SIZE);
ASSERT_THAT(vec != NULL);
for (i = 0; i < PERSONS_ARR_SIZE; ++i)
{
    ASSERT_THAT((res = VectorAppend(vec, (void *)(arr[i]))) == VECTOR_SUCCESS);
}
for (i = 0; i < PERSONS_ARR_SIZE; ++i)
{
    ASSERT_THAT((res = VectorRemove(vec, (void *)(&deletedPerson))) == VECTOR_SUCCESS);
    ASSERT_THAT(deletedPerson != NULL);
}
/*PrintVectorResult(res);*/
VectorDestroy(&vec, NULL);
free(arr);
ASSERT_THAT(NULL == vec);
ASSERT_THAT(NULL == vec);
END_UNIT

UNIT(VectorGet__Item__Test)
int i;
Vector *vec = NULL;
VectorResult res;
Person **arr = NULL;
Person *getPerson = NULL;
int shufled[PERSONS_ARR_SIZE];

arr = (Person **)malloc(PERSONS_ARR_SIZE * sizeof(Person *));
ASSERT_THAT(arr != NULL);

for (i = 0; i < PERSONS_ARR_SIZE; ++i)
{
    arr[i] = (Person *)malloc(1* sizeof(Person));
    shufled[i] = i;
}

ShuffleArray(shufled, PERSONS_ARR_SIZE);


getPerson = (Person*)malloc(1*sizeof(Person));

vec = VectorCreate(INITIAL_SIZE, BLOCK_SIZE);
ASSERT_THAT(vec != NULL);

for (i = 0; i < PERSONS_ARR_SIZE; ++i)
{
    ASSERT_THAT((res = VectorAppend(vec, (void *)(arr[shufled[i]]))) == VECTOR_SUCCESS);

}


for (i = 1; i <= PERSONS_ARR_SIZE; ++i)
{
    ASSERT_THAT((res = VectorGet(vec, (size_t)i ,(void *)(&getPerson))) == VECTOR_SUCCESS);
    ASSERT_THAT( getPerson == arr[shufled[i-1]]);
}


VectorDestroy(&vec, PersonDestroy);

free(arr);
ASSERT_THAT(NULL == vec);
END_UNIT

UNIT(Vector_Get_Set__Items__Test)

int i;
Vector *vec = NULL;
VectorResult res;
Person **arr = NULL;
Person *getPerson = NULL;

int shufled[ARR_HUGE_SIZE];

arr = (Person **)malloc(ARR_HUGE_SIZE * sizeof(Person *));
ASSERT_THAT(arr != NULL);

for (i = 0; i < ARR_HUGE_SIZE; ++i)
{
    arr[i] = (Person *)malloc(1 * sizeof(Person));
    arr[i]->age = rand() % 80 ;
    arr[i]->edu = rand() % 20;
    shufled[i] = i;
}

ShuffleArray(shufled, ARR_HUGE_SIZE);

getPerson = (Person *)malloc(1 * sizeof(Person));


vec = VectorCreate(INITIAL_SIZE, BLOCK_SIZE);
ASSERT_THAT(vec != NULL);

for (i = 0; i < ARR_HUGE_SIZE; ++i)
{
    ASSERT_THAT((res = VectorAppend(vec, (void *)(arr[shufled[i]]))) == VECTOR_SUCCESS);
}

for (i = 1; i <= ARR_HUGE_SIZE; ++i)
{
    ASSERT_THAT((res = VectorGet(vec, (size_t)i, (void *)(&getPerson))) == VECTOR_SUCCESS);
    ASSERT_THAT(getPerson == arr[shufled[i - 1]]);
    ASSERT_THAT((res = VectorSet(vec, (size_t)i, (void *)(getPerson))) == VECTOR_SUCCESS);
}
/*VectorForEach(vec, PrintPersonInColor, NULL);*/
VectorDestroy(&vec, PersonDestroy);

free(arr);
ASSERT_THAT(NULL == vec);
END_UNIT

UNIT(Vector_For_Each_Test)

    int i;
    /*size_t index=1;
    */
    Vector *vec = NULL;
    VectorResult res;
    Person **arr = NULL;
    
   
    arr = (Person **)malloc(PERSONS_ARR_SIZE * sizeof(Person *));
    
    ASSERT_THAT(arr != NULL);
    
    for (i = 0; i < PERSONS_ARR_SIZE; ++i)
    {
        arr[i] = (Person *)malloc(PERSONS_ARR_SIZE * sizeof(Person));
    }
    vec = VectorCreate(INITIAL_SIZE, BLOCK_SIZE);
    ASSERT_THAT(vec != NULL);
    for (i = 0; i < PERSONS_ARR_SIZE-1; ++i)
    {
        ASSERT_THAT((res = VectorAppend(vec, (void *)(arr[i]))) == VECTOR_SUCCESS);
    }

    arr[i]->age=55;
    arr[i]->edu=32;
    strcpy(arr[i]->name,"ADHAM");

    ASSERT_THAT((res = VectorAppend(vec, (void *)(arr[i]))) == VECTOR_SUCCESS);
    /*ASSERT_THAT((res = VectorSet(vec, 9 ,(void *)(arr[i]))) == VECTOR_SUCCESS);
    */
    
    /*
    index = VectorForEach(vec, PrintPerson, (void*)("ADHAM"));
    ASSERT_THAT(index == i+1);
    */
    VectorDestroy(&vec, NULL);
    ASSERT_THAT(NULL == vec);
END_UNIT

/*
 * Main function Macro 
 **/

TEST_SUITE(Generic-Vector-test)
TEST(ssssrand)
    TEST(Vector__Create__Test)
    TEST(Vector__Create__Initial_Zero__Test)
    TEST(Create_Normal_Inputs_Test)
    TEST(Create_Zero_Both_Inputs_Test)
    TEST(Create_Zero_Initial_Size_Test)
    TEST(Create_Zero_IncrBlock_Test)
    TEST(Create_Huge_Initial_Allocation_Test)

    TEST(Vector__Destroy__Null__Fp__Test)
    TEST(Vector__Destroy__With__FuncPointer__Test)
    TEST(Vector__Append__Few__Test)
    TEST(Vector__Append__With__Initial__Size__Zero__Test)
    TEST(Vector__Append__Many__Test)
    TEST(Vector__Append__NULL__Test)
    TEST(Vector__Append__NULL__Vector_Test)
    TEST(Vector__Append__With__Realloc__Test)
    TEST(Vector__Append__OverFlow__Test)
    TEST(Vector__Append__Invalid__Data__In__Test)
    TEST(Vector__Remove__One__Item__Test)
    TEST(Vector__Remove__Many__With__Realloc__Test)
    TEST(Vector__Remove__EmptyVector__Test)
    TEST(VectorGet__Item__Test)
    TEST(Vector_Get_Set__Items__Test)
    TEST(Vector_For_Each_Test)
END_SUITE
