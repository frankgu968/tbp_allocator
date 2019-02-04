/*
 * pool_alloc.c
 *
 *  Created on: Feb. 4, 2019
 *      Author: Frank Gu
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "pool_alloc.h"

static uint8_t g_pool_heap[MAX_HEAP_SIZE];

bool pool_init(size_t* block_sizes, size_t block_size_count) {
  // TODO
}

void* pool_malloc(size_t n){
  // TODO
}

void pool_free(void* ptr) {
  // TODO
}
