/*
 * pool_alloc.h
 *
 *  Created on: Feb. 4, 2019
 *      Author: Frank Gu
 */

#ifndef POOL_ALLOC_H_
#define POOL_ALLOC_H_

// Max number of bytes in heap data structure
#define MAX_HEAP_SIZE 65536

bool pool_init(size_t* block_sizes, size_t block_size_count);

void* pool_malloc(size_t n);

void pool_free(void* ptr);

#endif /* POOL_ALLOC_H_ */
