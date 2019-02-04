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
	return EXIT_SUCCESS;
}
