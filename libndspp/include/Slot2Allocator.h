/*
  Copyright (C) 2025 Antonio Niño Díaz

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef Slot2Allocator_h_seen
#define Slot2Allocator_h_seen

#include <cassert>
#include <cstdlib>
#include "libnds.h"

typedef struct Chunk
{
    struct Chunk *previous; // Pointer to previous chunk. NULL if this is the first one
    struct Chunk *next;     // Pointer to next chunk. NULL if this is the last one
    uint32_t size;          // Size of the chunk in bytes
    uint16_t used;          // Used or free
}
Chunk;

// Ensure that the size is a multiple of 16 bits so that it works well in
// external Slot-2 RAM.
static_assert((sizeof(Chunk) & 1) == 0);

class Slot2Allocator
{
public:
    Slot2Allocator();
    ~Slot2Allocator();

    void *alloc(size_t size);
    int free(void *pointer);

    void get_usage(size_t *used_bytes, size_t *free_bytes, size_t *total_bytes);

    static Slot2Allocator & instance();

private:
    Chunk *first_chunk { NULL };

    static const size_t CHUNK_MIN_SIZE = 16; // In bytes

    Chunk *split_chunk(Chunk *this_chunk, size_t this_size);
    void *chunk_to_user_ptr(Chunk *chunk);
    Chunk *user_ptr_to_chunk(void *ptr);
};

#endif
