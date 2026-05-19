# ccore

Cross platform C++ core library that provides low level types, memory and allocation primitives, math/bit utilities, containers for fixed-size allocation, error handling, formatting, random/hash utilities, and text/rune helpers.

ccore is the foundational layer used by other repositories in this ecosystem.

## What Is In source/main

Public headers are under source/main/include/ccore and implementations are under source/main/cpp.

Main modules:

- Platform and type system
	- c_target.h
	- c_limits.h
	- c_endian.h
- Memory and allocation
	- c_memory.h
	- c_allocator.h
	- c_arena.h
- Algorithms and low level utilities
	- c_math.h
	- c_qsort.h
	- c_binary_search.h
	- c_array.h
- Bitset allocation structures
	- c_binmap1.h
	- c_duomap1.h
	- c_bin.h
	- c_bindex.h
- Runtime utilities
	- c_debug.h
	- c_error.h
	- c_hash.h
	- c_random.h
	- c_callback.h
	- c_defer.h
	- c_stream.h
- Text and formatting
	- c_runes.h
	- c_va_list.h
	- c_printf.h

## Key Features

- Consistent primitive and platform types for 32-bit and 64-bit targets.
- Configurable debug/assert infrastructure with custom assert handlers.
- Abstract allocator interface plus helper construction/allocation functions.
- Virtual-memory-backed arena allocator with save/restore points.
- Fixed-size bin allocators and compact indexed bins for high-volume object pools.
- Hierarchical binmaps and duomaps for fast bit tracking and searching.
- Generic and typed quicksort plus binary search helpers.
- Hashing utilities for raw data and strings, including lowercase string hashes.
- Random number interfaces with implementations for xor-based and seed-based generators.
- Type-aware formatting and vararg wrappers used by printf-style functions.
- Rune/string helpers for ascii, utf8, utf16, utf32 and ucs2.
- Minimal stream interfaces for reader, writer, and seekable streams.

## Quick Usage

Include the header(s) you need and use namespace ncore.

### Sorting

```cpp
#include "ccore/c_qsort.h"

ncore::s32 values[] = {7, 2, 9, 1};
ncore::nsort::sort(values, 4);
```

### Binary Search

```cpp
#include "ccore/c_binary_search.h"

const ncore::u32 sorted[] = {3, 8, 11, 27};
ncore::s32 idx = ncore::g_BinarySearch(sorted, 4, (ncore::u32)11);
// idx == 2, or -1 when not found
```

### Arena Allocation

```cpp
#include "ccore/c_arena.h"

ncore::arena_t* arena = ncore::narena::new_arena(8 * cGB, 16 * cMB);
void* mark = ncore::narena::current_address(arena);

void* ptr = ncore::narena::alloc(arena, 1024);
ncore::narena::restore_address(arena, mark);

ncore::narena::destroy(arena);
```

### Fixed-Size Bin

```cpp
#include "ccore/c_bin.h"

ncore::nbin::bin_t* bin = ncore::nbin::make_bin(64, 64 * cKB);
void* p = ncore::nbin::alloc(bin);
ncore::nbin::free(bin, p);
ncore::nbin::destroy(bin);
```

### Callback Delegate

```cpp
#include "ccore/c_callback.h"

struct Dog { void Bark(int v) { volume = v; } int volume = 0; };
Dog d;

ncore::callback_t<void, int> cb;
cb.Reset(&d, &Dog::Bark);
cb(42);
```

## Build And Test

This repository is generated/configured via the ccode package definition in package/package.go.

Typical workflow:

1. Generate project/build files:
	 - go run ccore.go
2. Build and run unit tests with your generated build setup (commonly tundra-based in this workspace).

Unit tests are under source/test/cpp and cover arena, sorting, binary search, bin/bindex, binmap/duomap, hash, callback, endian, memory, and error handling.

## Notes

- c_printf uses the type-safe vararg wrapper in c_va_list, so mismatched format/argument types are handled more safely than raw C varargs.
- c_bindex free can return the owner index of a moved element when compaction occurs. Caller updates index mappings accordingly.
- Arena allocations are bulk-managed; individual deallocate is intentionally a no-op for arena_alloc_t.

