# Tunable Block Pool Allocator
Author: Frank Gu  
Date: Feb 4, 2019

## Overview
The tunable block pool allocator is a simple memory allocator that divides a memory block into various regions; each of which contains equal-sized blocks that can be atomically allocated. This is an extension to common block pool allocators which utilizes fixed block sizes.

## Usage
#### Prerequisites:
- autotools
- (optional)doxygen
- build-essentials

See test examples under `tests/`.
See demos under `demo/`.

To build documentation:
```bash
cd docs
doxygen doxygen.config
```

To build:
```bash
autoreconf --install   # See note below!
mkdir build && cd build
../configure
make all
make check
```

Note:
On some Homebrew-installed machines, the autotools may need to be configured using:
```bash
aclocal
autoreconf -f -i -Wall,no-obsolete
```
See this [link](https://github.com/Homebrew/legacy-homebrew/issues/5117).

## Limitations
| Subject | Limitation |
|---------|------------|
| block_size_count | Min: 1, Max: 255 |
| block_sizes item | Min 1, Max: 65535 |
| Allocation request size | Min: 0, Max 65535 | 

## Design Considerations
#### Assumptions
1. Heap space wastage shall be minimized
2. Memory slice being freed/allocated doesn't require zeroing
3. The entire space given by `g_pool_allocator` doesn't need to be solely used for object storage
4. There exists an external error handler

#### Constraints
1. Cannot use malloc; memory footprint must be fixed.

#### Requirements
1. Allocator provides a pointer to the smallest block that will fit the request
2. Allocator functions with both ordered and unordered `block_sizes` lists, given that its entries are valid
3. Allocator frees any valid pointer and recycles the previously allocated space for future allocations
4. Allocator should provide detailed debug messages to console, but not be included for production.

The single constraint rules out the use of dynamically allocated linked lists to hold memory state data. Since there is no information regarding the specific use-case of this allocator, no assumption will be made regarding the distribution of object sizes. The allocator will attempt to allocate relatively equal numbers of slices per block-size.

## Implementation
#### Initialization
In fulfillment of the design assumptions on space and efficiency, the allocator will use additional heap tp only store simple state variables (# of blocks, initialized flag, relative address of some metadata structures).

The metadata structures consists of a copy of `block_sizes`, offset from base address, base addresses of each block-size region. Within each block-size region, the first *offset* number of bytes is dedicated to a occupation map that defines which slots within that region has been allocated in a one-hot format. See below for a visualization:

```
Base: g_heap_pool    Offset  
|----------------| < block_sizes_list  
| base_sizes     |   0x0  
|----------------| < block_offset_list  
| block_offsets  |   sizeof(uint16_t) * num_block_size  
|----------------| < block_base_addr  
| base_addresses |   sizeof(uint8_t*) * num_block_size  
|----------------| < block_base_addr[0] (Smallest block-size region start)
| occupation_map |
|----------------| < block_base_addr[0]+block_offset_list[0]
| Region with m  |   block_size * capacity
|  size n slices |
|----------------| < block_base_addr[n]
|   ...    ...   |
|----------------| <- alloc_end_addr
```

`pool_init` will attempt to establish the above structure in g_heap_pool.
4 heap region base addresses (`block_sizes_list`, `block_offset_list`, `block_base_addr`, `alloc_end_addr`) are requested as a trade-off of heap usage and speed; calculating these addresses can become a large performance detriment if malloc/frees occur very often.

#### Allocation
During allocation, the allocator will simply traverse the block_sizes_list and find the smallest block size that will fit the requested size. The block-size region's occupation map (at block_base_addr[i]) is then traversed bit-wise to check for the first available free slot (bit=0); see `findFreeSlot` function.

If a free slot is found, then the allocator will mark that bit as occupied, and return the address of the free slot.

If a free slot is not found in this block-size region, the allocator will continue to the next block-size region and repeat the search process.

When all block-regions have been traversed, and no free slot is found, then the allocation fails due to lack of space.
