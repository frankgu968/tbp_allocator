#include <config.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <check.h>

#include "../src/pool_alloc.h"

// START Test Suite: Normal memory initialization
/*
 * Test: pool_init_unsorted
 * Description: Normally initialize a memory region with an unsorted block_sizes list
 * Precondition: Unsorted block_sizes list
 * Postcondition: True (Successful allocation)
 */
START_TEST (pool_init_unsorted)
{
  size_t sizes_list[5] = {24,12,8,16,20};
  size_t num_elements = 5;

  bool result = pool_init(sizes_list, num_elements);
  ck_assert(result);
}
END_TEST

/*
 * Test: pool_init_sorted
 * Description: Normally initialize a memory region with a sorted block_sizes list
 * Precondition: Sorted block_sizes list
 * Postcondition: True (Successful allocation)
 */
START_TEST (pool_init_sorted)
{
  size_t sizes_list[3] = {8, 12, 16};
  size_t num_elements = 3;

  bool result = pool_init(sizes_list, num_elements);
  ck_assert(result);
}
END_TEST
// END Test Suite: Normal memory initialization

// START Test Suite: Bad initialization parameters
/*
 * Test: list_too_long
 * Description: block_sizes list is too long
 * Precondition: block_size_count > UCHAR_MAX
 * Postcondition: False (Failed allocation)
 */
START_TEST (list_too_long)
{
  size_t sizes_list[1] = {1};
  size_t num_elements = 256;

  bool result = pool_init(sizes_list, num_elements);
  ck_assert(!result);
}
END_TEST

/*
 * Test: list_size_zero
 * Description: List size is 0
 * Precondition: block_size_count = 0
 * Postcondition: False (Failed allocation)
 */
START_TEST (list_size_zero)
{
  size_t sizes_list[3] = {8, 12, 16};
  size_t num_elements = 0;

  bool result = pool_init(sizes_list, num_elements);
  ck_assert(!result);
}
END_TEST

/*
 * Test: null_list_pointer
 * Description: List pointer is null
 * Precondition: block_sizes = NULL
 * Postcondition: False (Failed allocation)
 */
START_TEST (null_list_pointer)
{
  size_t* sizes_list = NULL;
  size_t num_elements = 3;

  bool result = pool_init(sizes_list, num_elements);
  ck_assert(!result);
}
END_TEST

/*
 * Test: invalid_list_item
 * Description: block_sizes contain invalid block sizes
 * Precondition: block_sizes = {0, 65537}
 * Postcondition: False (Failed allocation)
 */
START_TEST (invalid_list_item)
{
  size_t sizes_list[2] = {0, 65537};
  size_t num_elements = 2;

  bool result = pool_init(sizes_list, num_elements);
  ck_assert(!result);
}
END_TEST
// END Test Suite: Bad initialization parameters

// START Test Suite: Tricky memory block list combinations
/*
 * Test: total_too_large
 * Description: block_sizes contain block sizes that sum up to greater than allocable area
 * Precondition: block_sizes = {8, 16, 32, 66, 65500}
 * Postcondition: False (Failed allocation)
 */
START_TEST (total_too_large)
{
  size_t sizes_list[5] = {8, 16, 32, 66, 65500};
  size_t num_elements = 5;

  bool result = pool_init(sizes_list, num_elements);
  ck_assert(!result);
}
END_TEST
// END Test Suite: Tricky memory block list combinations

Suite * pool_alloc_suite(void)
{
    Suite* s = suite_create("pool_init");

    TCase* tc_normal_init = tcase_create("Normal memory initialization");
    tcase_add_test(tc_normal_init, pool_init_unsorted);
    tcase_add_test(tc_normal_init, pool_init_sorted);
    suite_add_tcase(s, tc_normal_init);

    TCase* tc_bad_init_params = tcase_create("Bad initialization parameters");
    tcase_add_test(tc_bad_init_params, list_too_long);
    tcase_add_test(tc_bad_init_params, list_size_zero);
    tcase_add_test(tc_bad_init_params, null_list_pointer);
    tcase_add_test(tc_bad_init_params, invalid_list_item);
    suite_add_tcase(s, tc_bad_init_params);

    TCase* tc_tricky_combinations = tcase_create("Tricky memory block list combinations");
    tcase_add_test(tc_tricky_combinations, total_too_large);
    suite_add_tcase(s, tc_tricky_combinations);

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
