/*
 ============================================================================
 Name        : main.c
 Author      : Frank Gu
 Version     :
 Copyright   : MIT 2018
 Description : Tunable memory allocator demonstrator
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define NUM_ELEMENTS 5

int main(void) {
	printf("Tunable Block Pool Allocator by Frank Gu\n");

	size_t sizes_list[NUM_ELEMENTS] = {24,12,8,16,20};
  size_t list_num = NUM_ELEMENTS;
  bool result = pool_init(sizes_list, list_num);

  if(result){
    printf("Pool init successful!\n");
    printMemory();

  } else {
    printf("Pool init failed...\n");
  }

	void* alloc_result = pool_malloc(17);
	void* alloc_result2 = pool_malloc(18);
	void* alloc_result3 = pool_malloc(19);
	if(alloc_result != NULL){
		printf("Allocated pointer: %p\n", alloc_result);
		printMemory();
		pool_free(alloc_result2);
		printMemory();
	} else {
		printf("Could not get allocation!");
	}

	return EXIT_SUCCESS;
}
