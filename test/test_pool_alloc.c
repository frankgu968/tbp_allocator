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

Suite * pool_init_suite(void)
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

// START Test Suite: pool_malloc_suite
/*
 * Test: single_alloc
 * Description: Obtain a pointer at the correct location by validating the relative position of offset_list and base_addr
 * Precondition: block_sizes = {32, 64}, request size 20
 * Postcondition: A pointer with the correct relative location
 */
START_TEST (single_alloc)
{
  size_t sizes_list[2] = {32, 64};
  size_t num_elements = 2;

  bool result = pool_init(sizes_list, num_elements);
  ck_assert(result);
  uint8_t* result_ptr = (uint8_t*) pool_malloc(20);

  // With MAX_HEAP_SIZE = 65536, we should get
  // 681 slices (9 bytes) - 32B
  // 680 slices (8 bytes) - 64B
  void* base_addr_32 = result_ptr - 86;
  void* addr_list = base_addr_32 - num_elements * sizeof(uint8_t*);
  void* offset_addr = addr_list - 4;
  ck_assert_ptr_eq(*((uint8_t**)addr_list), base_addr_32);
  ck_assert_uint_eq(*((uint16_t*)offset_addr), 86);
}
END_TEST

/*
 * Test: multi_size_alloc
 * Description: Obtain two pointers from different blocks at the correct locations by validating the relative positions of offset_list and base_addr
 * Precondition: block_sizes = {32, 64}, request size 20 and 40
 * Postcondition: A pointer with the correct relative location
 */
START_TEST (multi_size_alloc)
{
  size_t sizes_list[2] = {32, 64};
  size_t num_elements = 2;

  bool result = pool_init(sizes_list, num_elements);
  ck_assert(result);
  void* ptr_20 = (uint8_t*) pool_malloc(20);
  void* ptr_40 = (uint8_t*) pool_malloc(40);

  // With MAX_HEAP_SIZE = 65536, we should get
  // 681 slices (9 bytes) - 32B
  // 680 slices (8 bytes) - 64B

  // Ensure ptr_20 is allocated correctly
  void* base_addr_32 = ptr_20 - 86;
  void* addr_list = base_addr_32 - num_elements * sizeof(uint8_t*);
  void* offset_addr = addr_list - 4;
  ck_assert_ptr_eq(*((uint8_t**)addr_list), base_addr_32);
  ck_assert_uint_eq(*((uint16_t*)offset_addr), 86);

  // Ensure ptr_40 is allocated correctly
  void* base_addr_64 = ((uint8_t**)addr_list)[1];
  uint16_t offset_64 = *((uint16_t*)(offset_addr + sizeof(uint16_t)));
  ck_assert_uint_eq(offset_64, 85);
  ck_assert_ptr_eq(base_addr_64 + offset_64, ptr_40);
}
END_TEST

/*
 * Test: fill_block_alloc
 * Description: Continuously request 20 for 682 units and verify the 682nd unit is in the 64B block region
 * Precondition: block_sizes = {32, 64}, request size 20 for 682 times
 * Postcondition: A pointer with the correct relative location
 */
START_TEST (fill_block_alloc)
{
  size_t sizes_list[2] = {32, 64};
  size_t num_elements = 2;

  bool result = pool_init(sizes_list, num_elements);
  ck_assert(result);
  void* ptr_in_32 = pool_malloc(20); // Used to easily calculate offsets
  for(size_t i = 1; i < 681; i++){
    pool_malloc(20);
  }
  void* result_ptr = pool_malloc(20);
  // Calculate where the base address of the 64 B region is and compare
  // With MAX_HEAP_SIZE = 65536, we should get
  // 681 slices (9 bytes) - 32B
  // 680 slices (8 bytes) - 64B

  // Ensure ptr_20 is allocated correctly
  void* base_addr_32 = ptr_in_32 - 86;
  void* addr_list = base_addr_32 - num_elements * sizeof(uint8_t*);
  void* offset_addr = addr_list - 4;

  // Ensure ptr_40 is allocated correctly
  void* base_addr_64 = ((uint8_t**)addr_list)[1];
  uint16_t offset_64 = *((uint16_t*)(offset_addr + sizeof(uint16_t)));
  ck_assert_uint_eq(offset_64, 85);
  ck_assert_ptr_eq(base_addr_64 + offset_64, result_ptr);
}
END_TEST

/*
 * Test: no_space_left
 * Description: Check attempt to malloc with no space left
 * Precondition: block_sizes = {32, 64}, request size 40, 681 times
 * Postcondition: NULL pointer returned
 */
START_TEST (no_space_left)
{
  size_t sizes_list[2] = {32, 64};
  size_t num_elements = 2;

  bool result = pool_init(sizes_list, num_elements);
  ck_assert(result);
  for(size_t i = 0; i < 680; i++){
    pool_malloc(40);
  }
  void* result_ptr = pool_malloc(40);
  ck_assert_ptr_eq(result_ptr, NULL);
}
END_TEST

/*
 * Test: zero_size
 * Description: Attempt to allocate a 0-sized chunk
 * Precondition: block_sizes = {32, 64}, request size 0
 * Postcondition: NULL pointer returned
 */
START_TEST (zero_size)
{
  size_t sizes_list[2] = {32, 64};
  size_t num_elements = 2;

  bool result = pool_init(sizes_list, num_elements);
  ck_assert(result);

  void* result_ptr = pool_malloc(0);
  ck_assert_ptr_eq(result_ptr, NULL);
}
END_TEST

/*
 * Test: too_large
 * Description: Attempt to allocate a chunk larger than the largest block size
 * Precondition: block_sizes = {32, 64}, request size 65
 * Postcondition: NULL pointer returned
 */
START_TEST (too_large)
{
  size_t sizes_list[2] = {32, 64};
  size_t num_elements = 2;

  bool result = pool_init(sizes_list, num_elements);
  ck_assert(result);

  void* result_ptr = pool_malloc(65);
  ck_assert_ptr_eq(result_ptr, NULL);
}
END_TEST
// END Test Suite: pool_malloc_suite

Suite * pool_malloc_suite(void)
{
  Suite* s = suite_create("pool_malloc");

  TCase* tc_normal_malloc = tcase_create("Normal memory requests");
  tcase_add_test(tc_normal_malloc, single_alloc);
  tcase_add_test(tc_normal_malloc, multi_size_alloc);
  tcase_add_test(tc_normal_malloc, fill_block_alloc);
  tcase_add_test(tc_normal_malloc, no_space_left);
  suite_add_tcase(s, tc_normal_malloc);

  TCase* tc_invalid_req = tcase_create("Invalid request size");
  tcase_add_test(tc_invalid_req, zero_size);
  tcase_add_test(tc_invalid_req, too_large);
  suite_add_tcase(s, tc_invalid_req);
  return s;
}

// START Test Suite: pool_free_suite
/*
 * Test: single_free
 * Description: Allocate and free a single pointer
 * Precondition: block_sizes = {1024, 7763}, request and free an allocation 'old_alloc'
 * Postcondition: Newly allocated block and previously allocated block should have same pointer value
 */
START_TEST (single_free)
{
  size_t sizes_list[2] = {1024, 7763};
  size_t num_elements = 2;

  bool result = pool_init(sizes_list, num_elements);
  ck_assert(result);
  void* old_alloc = pool_malloc(80);
  ck_assert_ptr_ne(old_alloc, NULL);
  pool_free(old_alloc);

  // A new allocation within the same block should take the exact same address as the old block
  void* new_alloc = pool_malloc(120);
  ck_assert_ptr_eq(new_alloc, old_alloc);
}
END_TEST

/*
 * Test: unaligned_ptr
 * Description: Unaligned pointer free should not occur
 * Precondition: Value at an unaligned address is written to 0xFF
 * Postcondition: Value should not change
 */
START_TEST (unaligned_ptr)
{
  size_t sizes_list[5] = {1024, 7763, 4, 7, 99};
  size_t num_elements = 5;

  bool result = pool_init(sizes_list, num_elements);
  ck_assert(result);
  void* result_ptr = pool_malloc(80);
  void* unaligned_ptr = result_ptr+1;
  *((uint16_t*)unaligned_ptr) = 0xFF;

  pool_free(unaligned_ptr);
  ck_assert_uint_eq(*((uint16_t*)unaligned_ptr), 0xFF);
}
END_TEST
// END Test Suite: pool_free_suite


Suite * pool_free_suite(void)
{
  Suite* s = suite_create("pool_free");

  TCase* tc_normal_free = tcase_create("Normal free");
  tcase_add_test(tc_normal_free, single_free);
  suite_add_tcase(s, tc_normal_free);
  
  return s;
}

int main(void)
{
    int number_failed;

    Suite* init_suite = pool_init_suite();
    SRunner* sr = srunner_create(init_suite);
    srunner_add_suite(sr, pool_malloc_suite());
    srunner_add_suite(sr, pool_free_suite());

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
