#include <config.h>
#include <stdlib.h>
#include <stdint.h>
#include <check.h>
#include "../src/pool_alloc.h"

START_TEST (tru_test)
{
  /* unit test code */
  ck_assert_int_eq(5, 5);
}
END_TEST

Suite * pool_alloc_suite(void)
{
    Suite *s;
    TCase *tc_core;
    TCase *tc_limits;

    s = suite_create("pool_alloc");

    /* Core test case */
    tc_core = tcase_create("Init");
    tcase_add_test(tc_core, tru_test);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = pool_alloc_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
