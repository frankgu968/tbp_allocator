/*
 * pool_alloc.c
 *
 *  Created on: Feb. 4, 2019
 *      Author: Frank Gu
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <limits.h>

#include "pool_alloc.h"

static uint8_t g_pool_heap[MAX_HEAP_SIZE];
static size_t num_block_size;
static bool f_pool_init = false;

// Convenience address holders
static uint16_t* block_sizes_list;
static uint16_t* block_offset_list;
static uint8_t** block_base_addr;
static uint8_t* alloc_end_addr;

bool pool_init(size_t* block_sizes, size_t block_size_count) {
  // Validate block_sizes list and its items
  assert(!f_pool_init); // Trap if the region has already been initialized
  if((block_sizes == NULL) || (block_size_count == 0) || (block_size_count > UCHAR_MAX + 1)) {
    // Invalid input parameters
    return false;
  }

  for(uint8_t i = 0; i < block_size_count; i++) {
    if((block_sizes[i] == 0) || (block_sizes[i] > MAX_HEAP_SIZE)){
      // Invalid list entry
      return false;
    }
  }

  uint8_t* heap_ptr = g_pool_heap;    // Pointer to current heap usage
  num_block_size = block_size_count;

  // Copy over block size list
  block_sizes_list = (uint16_t*)heap_ptr;
  for(uint8_t i = 0; i < num_block_size; i++){
    if(block_sizes[i] >= MAX_HEAP_SIZE) {
      // Invalid input
      return false;
    }
    *((uint16_t*)heap_ptr) = (uint16_t)(block_sizes[i]);
    heap_ptr += sizeof(uint16_t);
  }
  insertionSort(block_sizes_list, num_block_size);
  assert(heap_ptr < g_pool_heap + sizeof(uint8_t) * MAX_HEAP_SIZE);

  block_offset_list = (uint16_t*)heap_ptr;

  // Initialize temporary counts array in heap (use the offset list region)
  uint16_t* temp_block_counts = block_offset_list;
  for(uint8_t i = 0; i < num_block_size; i++){
    *((uint16_t*)heap_ptr) = 0;
    heap_ptr += sizeof(uint16_t);
  }
  assert(heap_ptr < g_pool_heap + sizeof(uint8_t) * MAX_HEAP_SIZE);

  // Available = MAX - block_size_list - offset_list - base_addr_list
  uint16_t available_bytes = MAX_HEAP_SIZE - num_block_size * (sizeof(uint16_t) + sizeof(uint16_t)+sizeof(uint8_t*));

  // Try to evenly allocate blocks
  while(available_bytes > 0) {
    // Iterate through block_sizes_list to create regions
    for(uint8_t i = 0; i < num_block_size; i++) {
      if(available_bytes >= block_sizes_list[i]) {
        if(temp_block_counts[i] % 8 == 0){
          // A new memory bitmap byte is needed
          // Make sure that the block fits with at least 1 more byte
          if(available_bytes > block_sizes_list[i]) {
            available_bytes -= 1;
          } else {
            break; // No more space to hold extra bitmap byte; forgo last block
          }
        }
        temp_block_counts[i] += 1;
        available_bytes -= block_sizes_list[i];
      } else {
        if(temp_block_counts[i] == 0){
          // ERROR: Not all block sizes could be allocated
          return false;
        } else if (available_bytes < block_sizes_list[0]){
          // Remaining space cannot fit the smallest block
          available_bytes = 0;
          break;
        }
      }
    }
  }

  block_base_addr = (uint8_t **) heap_ptr;
  heap_ptr += sizeof(uint8_t*) * num_block_size;

  // Prepare memory regions
  for(uint8_t i = 0; i < num_block_size; i++) {
    uint16_t occmap_num_bytes = temp_block_counts[i] / 8;
    uint8_t occmap_remainder = temp_block_counts[i] % 8;

    ((uint8_t**)block_base_addr)[i] = heap_ptr;

    // Populate memory occupation map
    for(uint16_t j = 0; j < occmap_num_bytes; j++) {
      ((uint8_t*)heap_ptr)[j] = 0;
    }

    // Calculate occupied map offset and populate last byte of occupation map
    if(occmap_remainder > 0) {
      occmap_num_bytes += 1;
      uint8_t remainder_map = 0;
      for(int8_t k = occmap_remainder - 1; k >= 0 ; k--){
        remainder_map |= (0x1 << k);
      }
      ((uint8_t*)heap_ptr)[occmap_num_bytes - 1] = ~remainder_map;
    }

    heap_ptr += sizeof(uint8_t) * occmap_num_bytes + temp_block_counts[i] * block_sizes_list[i];
    // Set occupied map offset from block_base_addr
    block_offset_list[i] = occmap_num_bytes;
  }
  alloc_end_addr = heap_ptr;
  f_pool_init = true; // Pool is initialized
  return true;
}


void* pool_malloc(size_t n){
  // TODO
}

void pool_free(void* ptr) {
  // TODO
}

// Helpers
void insertionSort(uint16_t arr[], const uint8_t n) {
  int16_t i, j;
  uint16_t key;
  for (i = 1; i < n; i++) {
    key = arr[i];
    j = i-1;

    /* Move elements of arr[0..i-1], that are
    greater than key, to one position ahead
    of their current position */
    while (j >= 0 && arr[j] > key) {
      arr[j+1] = arr[j];
      j = j-1;
    }
    arr[j+1] = key;
  }
}

void printMemory() {
  printf("Region: BlockSize ----------------------------------------\n");
  printf("Start address: %p\n", block_sizes_list);
  for(size_t i = 0; i < num_block_size; i++) {
    printf("Block: %d B\n", ((uint16_t*)block_sizes_list)[i]);
  }
  printf("\n");

  printf("Region: Offset -------------------------------------------\n");
  printf("Start address: %p\n", block_offset_list);
  for(size_t i = 0; i < num_block_size; i++) {
    printf("Offset: %d B\n", ((uint16_t*)block_offset_list)[i]);
  }
  printf("\n");

  printf("Region: Offset -------------------------------------------\n");
  printf("Start address: %p\n", block_base_addr);
  for(size_t i = 0; i < num_block_size; i++) {
    printf("Base: %p\n", ((uint8_t**)block_base_addr)[i]);
  }
  printf("\n");

  for(uint16_t i = 0; i < num_block_size; i++) {
    printf("Slice %d: %dB Slices =================================\n", i + 1, ((uint16_t*)block_sizes_list)[i]);

    char occ_map_str[65536] = "";
    uint16_t capacity = 0;
    for(uint16_t j = 0; j < ((uint16_t*)block_offset_list)[i]; j++){
      uint8_t occ_map_byte = *((uint8_t*)(block_base_addr[i]) + j);
      if(occ_map_byte == 0){
        strcat(occ_map_str, "00000000");
        capacity += 8;
      } else if (occ_map_byte == 255) {
        strcat(occ_map_str, "11111111");
      } else {
        for (uint8_t k = 0; k < 8; k++) {
          if((occ_map_byte & 0x1) == 1){
            strcat(occ_map_str, "1");
          } else {
            strcat(occ_map_str, "0");
            capacity += 1;
          }
          occ_map_byte = occ_map_byte >> 1;
        }
      }
    }
    printf("Capacity: %d\n", capacity);
    printf("Alloc Start: %p\n", ((uint8_t**)block_base_addr)[i]);
    printf("Alloc End: %p\n", ((uint8_t**)block_base_addr)[i] + ((uint16_t*)block_offset_list)[i]*sizeof(uint8_t) + capacity *((uint16_t*)block_sizes_list)[i]);
    printf("Occ Map: %s\n", occ_map_str);
    printf("\n");
  }

  printf("Heap allocation end address: %p\n\n", alloc_end_addr);
}
