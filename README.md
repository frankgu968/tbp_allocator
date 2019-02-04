# Tunable Block Pool Allocator
Author: Frank Gu  
Date: Feb 4, 2019

## Overview
The tunable block pool allocator is a simple memory allocator that divides a memory block into various regions; each of which contains equal-sized blocks that can be atomically allocated. This is an extension to common block pool allocators which utilizes fixed block sizes.

## Usage
See documentation under `docs/`.  
See test examples under `tests/`.
See demos under `demo/`.

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

The single constraint rules out the use of dynamically allocated linked lists to hold memory state data.

## Implementation
