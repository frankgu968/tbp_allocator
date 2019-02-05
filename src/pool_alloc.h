/*
 * pool_alloc.h
 *
 *  Created on: Feb. 4, 2019
 *      Author: Frank Gu
 */

#ifndef POOL_ALLOC_H_
#define POOL_ALLOC_H_

#include <stdbool.h>

// Max number of bytes in heap data structure
#define MAX_HEAP_SIZE 65536

/** @brief heap initializer to populate metadata structures for future allocation.
    Will assert trap if initialization already completed once; or if the given configuration of the block_sizes list cannot fit the heap region given to the allocator.
    @param block_sizes A list of block sizes to be allocated
    @param block_size_count Length of the block_sizes list
    @return bool Success status of initialization
*/
bool pool_init(size_t* block_sizes, size_t block_size_count);

/** @brief Memory allocator
    Will assert trap if pool is not initialized
    @param n number of bytes requested
    @return void* Pointer to allocated area; NULL if allocation failed
*/
void* pool_malloc(size_t n);

/** @brief Frees allocated memory
    Will assert trap if pool is not initialized or if the pointer is either out-of-bounds of the allocation area or unaligned
    @param ptr pointer to be freed
*/
void pool_free(void* ptr);

// Helper functions
/** @brief Attempts to find a free slot within a given block-size region
    @param b_addr base address of the block-size region
    @param blk_free_loc pointer to a uint16_t variable that can store the first slot number that is free in the block-size region's occupation map
    @param block_size_idx index of the block_sizes_list to indicate which block-size region to search in
    @return bool true if free slice found; false otherwise
*/
bool findFreeSlot(uint8_t* b_addr, uint16_t* blk_free_loc, uint8_t block_size_idx);

/** @brief In-place insertion sort of a given list
    @param a Pointer to array to be sorted
    @param size size of the array
*/
void insertionSort(uint16_t* a,const uint8_t size);

/** @brief Diagnostic function that will make a detailed print of the heap pool regions. This function will also display each block-size region's occupation map and capacity status. 
*/
void printMemory();

#endif /* POOL_ALLOC_H_ */
