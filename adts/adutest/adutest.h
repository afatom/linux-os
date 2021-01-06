#pragma once

#include <stdio.h>
typedef int (*ut)(void);

#define MAX_TESTS_NUM (64)




#define UNIT_START(_unit_name_)  \
    int _unit_name_(void){ \
        printf("%p\n",_unit_name_);\
        test_array[0] = _unit_name_; \
        printf("%p %p\n",test_array,test_array[0]); \

#define UNIT_END }



#define INIT_ADUTEST() \
    ut test_array[]={ \

#define REGISTER_UNIT(unit) (unit), 

#define EXECUTE_ALL() \
    (0x0)};\
    for(int i = 0; test_array[i]!=0x0; ++i) \
    { \
        puts("in for");  \
        test_array[i](); \
    } \

extern const char *unit_name;
#define ADUTEST_INIT_UNIT(name) \
    unit_name = name; \
    printf("%s\n", name); \

#define ASSERT_INFO(expr) \
    if (expr) \
        {} \
    else \
        aFailed(F_NUM, __LINE__)

#define ASSERT_WARNING(expr) \
    if (expr) \
        {} \
    else \
        aFailed(F_NUM, __LINE__)

#define ASSERT_FATAL(expr,unit) \
    if (expr) \
    { \
        ut_passed(unit) \
    } \
    else \
        assert_fatal(__FILE__, __LINE__) \


void assert_info(const char *string, int line);
void assert_warning(const char *string, int line);
void assert_fatal(const char *string, int line);
void ut_passed(const char *utest);

