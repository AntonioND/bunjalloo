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

#include <cassert>
#include <cstdlib>
#include "Slot2Allocator.h"
#include "libnds.h"

// This allocator doesn't use any of the main RAM of the DS, it stores all
// information in the Slot-2 RAM.

Slot2Allocator & Slot2Allocator::instance()
{
    static Slot2Allocator s_instance;
    return s_instance;
}

Slot2Allocator::Slot2Allocator()
{
    if (!peripheralSlot2InitDefault())
        return;

    // TODO: Use this?
    // peripheralSlot2GetName()
    // peripheralSlot2RamBanks()

    void *start = (void *)peripheralSlot2RamStart();
    if (start == NULL)
        return;

    uintptr_t start_addr = (uintptr_t)start;
    uintptr_t end_addr = start_addr + peripheralSlot2RamSize();

    void *end = (void *)end_addr;

    if (end <= start)
        return;

    first_chunk = (Chunk *)start;

    first_chunk->previous = NULL;
    first_chunk->next = NULL;
    first_chunk->size = end_addr - start_addr - sizeof(Chunk);
    first_chunk->used = 0;
}

Slot2Allocator::~Slot2Allocator()
{
}

void *Slot2Allocator::chunk_to_user_ptr(Chunk *chunk)
{
    return ((uint8_t *)chunk) + sizeof(Chunk);
}

Chunk *Slot2Allocator::user_ptr_to_chunk(void *ptr)
{
    return (Chunk *)(((uint8_t *)ptr) - sizeof(Chunk));
}

// The function resizes this chunk to the provided size, and creates a new chunk
// with the remaining size.
//
// +-----------------+------+
// |      THIS       | NEXT |  Before
// +-----------------+------+
//
// +------+----------+------+
// | THIS |   NEW    | NEXT |  After
// +------+----------+------+
//
// It returns a pointer to the new chunk.
Chunk *Slot2Allocator::split_chunk(Chunk *this_chunk, size_t this_size)
{
    // Get next chunk and create a new one.

    Chunk *next = this_chunk->next;

    Chunk *new_chunk = (Chunk *)(((char *)this_chunk + sizeof(Chunk)) + this_size);

    // Update pointers in the linked list
    // ----------------------------------

    new_chunk->previous = this_chunk;
    this_chunk->next = new_chunk;

    if (next == NULL)
    {
        new_chunk->next = NULL;
    }
    else
    {
        new_chunk->next = next;
        next->previous = new_chunk;

        // It shouldn't be free because deallocating a chunk should merge it
        // with any free chunk next to it.
        assert(next->used);
    }

    new_chunk->size = this_chunk->size - this_size - sizeof(Chunk);
    this_chunk->size = this_size;

    return new_chunk;
}

void *Slot2Allocator::alloc(size_t size)
{
    if (size == 0)
        return NULL;

    // Pad size to CHUNK_MIN_SIZE if needed
    const size_t mask = CHUNK_MIN_SIZE - 1;
    size = (size + mask) &~ mask;

    Chunk *this_chunk = first_chunk;

    for ( ; this_chunk != NULL; this_chunk = this_chunk->next)
    {
        // Skip non-free chunks
        if (this_chunk->used)
            continue;

        // If this chunk doesn't have enough space, skip it.
        if (this_chunk->size < size)
            continue;

        // If we can't split the block because we can't fit another Chunk
        // struct, we're done.
        if (this_chunk->size <= (size + sizeof(Chunk)))
        {
            this_chunk->used = 1;
            return chunk_to_user_ptr(this_chunk);
        }

        // If we can split the chunk, do it.
        //
        // |      THIS       | NEXT |
        // +-----------------+------+  Before
        // |    NOT USED     | USED |
        //
        // | THIS |   NEW    | NEXT |
        // +------+----------+------+  After
        // | USED | NOT USED | USED |

        Chunk *new_chunk = split_chunk(this_chunk, size);

        // Flag this_chunk chunk as used and the new one as free
        this_chunk->used = 1;
        new_chunk->used = 0;

        return chunk_to_user_ptr(this_chunk);
    }

    // No more chunks... Not enough free space.
    return NULL;
}

int Slot2Allocator::free(void *pointer)
{
    Chunk *user_chunk = user_ptr_to_chunk(pointer);

    // Look for the chunk that corresponds to the given pointer
    Chunk *this_chunk = first_chunk;

    while (1)
    {
        // Check if we have reached the end without finding the chunk
        if (this_chunk == NULL)
            return -1;

        // If this is the chunk we're looking for, exit loop
        if (this_chunk == user_chunk)
            break;

        this_chunk = this_chunk->next;
    }

    // If the specified chunk is free, it can't be freed.
    if (!this_chunk->used)
        return -2;

    // Chunk found. Free it.
    this_chunk->used = 0;

    // Now, check if we can join this free chunk with the previous or the
    // next one
    Chunk *previous = this_chunk->previous;
    Chunk *next = this_chunk->next;

    // Check the previous one
    if (previous && !previous->used)
    {
        // We can join them
        //
        // | PREVIOUS |   THIS   | NEXT |
        // +----------+----------+------+
        // | NOT USED | NOT USED | ???? |
        //
        // |       PREVIOUS      | NEXT |
        // +---------------------+------+
        // |       NOT USED      | ???? |

        if (next)
        {
            // First, join the previous and the next
            next->previous = previous;
            previous->next = next;
        }
        else
        {
            previous->next = NULL;
        }

        // Expand the previous one
        previous->size += this_chunk->size + sizeof(Chunk);

        // Change the active chunk to try to join it with the next one.
        this_chunk = previous;
    }

    // Check the next one
    if (next && !next->used)
    {
        // We can join them
        //
        // |   THIS   |   NEXT   | NEXT NEXT |
        // +----------+----------+-----------+  Before
        // | NOT USED | NOT USED |   USED    |
        //
        // |         THIS        | NEXT NEXT |
        // +---------------------+-----------+  After
        // |       NOT USED      |   USED    |

        Chunk *next_next = next->next;

        if (next_next)
        {
            // Next Next should be used. If not, something bad is happening.
            assert(next_next->used);

            // First, join this chunk and the next next chunk
            next_next->previous = this_chunk;
            this_chunk->next = next_next;
        }
        else
        {
            this_chunk->next = NULL;
        }

        // Expand this node one
        this_chunk->size += next->size + sizeof(Chunk);
    }

    return 0;
}

void Slot2Allocator::get_usage(size_t *used_bytes, size_t *free_bytes, size_t *total_bytes)
{
    size_t _used = 0;
    size_t _free = 0;
    size_t _total = 0;

    Chunk *this_chunk = first_chunk;

    for ( ; this_chunk != NULL; this_chunk = this_chunk->next)
    {
        _total += sizeof(Chunk) + this_chunk->size;

        if (this_chunk->used)
            _used += this_chunk->size;
        else
            _free += this_chunk->size;
    }

    if (used_bytes)
        *used_bytes = _used;
    if (free_bytes)
        *free_bytes = _free;
    if (total_bytes)
        *total_bytes = _total;
}
