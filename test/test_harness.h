
#ifndef TEST_HARNESS_H
#define TEST_HARNESS_H

#include <time.h>
#include <stdio.h>

#define TEST(__group, __test) int __group ## _ ## __test(void) {

#define ASSERT_EQ(__expected, __actual)    \
    do \
    { \
         if (!((__expected) == (__actual))) \
         { \
             __test_harness_print_error(__FILE__, __LINE__, #__actual, (__actual), (__expected)); \
             return 1; \
         } \
    } while (0)

#define ASSERT_NE(__expected, __actual)    \
    do \
    { \
         if (((__expected) == (__actual))) \
         { \
             __test_harness_print_error(__FILE__, __LINE__, #__actual, (__actual), (__expected)); \
             return 1; \
         } \
    } while (0)

#define ASSERT_PTR_EQ(__expected, __actual)    \
    do \
    { \
         if (!((__expected) == (__actual))) \
         { \
             __test_harness_print_error(__FILE__, __LINE__, #__actual, (long)(__actual), (long)(__expected)); \
             return 1; \
         } \
    } while (0)

#define ASSERT_PTR_NE(__expected, __actual)    \
    do \
    { \
         if (((__expected) == (__actual))) \
         { \
             __test_harness_print_error(__FILE__, __LINE__, #__actual, (long)(__actual), (long)(__expected)); \
             return 1; \
         } \
    } while (0)


#define END_TEST(__group) {++ __group ## _pass_cnt;}   return 0; }


#define CALL_TEST(__group, __test) 									\
    do                                      						\
    {                                								\
        __test_harness_print_string("[ RUN      ] " #__group "." #__test "\n");	\
        clock_t start, end;                                         \
        start = clock();                                            \
        if (__group ## _ ## __test() == 0)  						\
        {                                   						\
            end = clock();                                          \
            __test_harness_print_string("[       OK ] " #__group "." #__test);			\
            double time_consumed = (double)(end - start) * 1000 / CLOCKS_PER_SEC; \
            printf(" (%.0f ms)\n\n", time_consumed);            \
        }                                   						\
        else                                						\
        {                                   						\
            __test_harness_print_string("[  FAILED  ] " #__group "." #__test "\n"); 				\
            ++ __group ## _fail_cnt;        						\
        }                                   						\
    } while (0);

#define TEST_GROUP(__group) int __group ## _pass_cnt = 0; int __group ## _fail_cnt = 0;

#define END_TEST_GROUP(__group) \
	{ \
		__test_harness_print_group_summary(#__group, __group ## _pass_cnt, __group ## _fail_cnt); \
    } \
    return 0;

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

void __test_harness_print_string(const char *str);
void __test_harness_print_error(const char *file, int line, const char *msg, long long actual, long long expected);
void __test_harness_print_group_summary(const char *group, int pass_cnt, int fail_cnt);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* TEST_HARNESS_H */
