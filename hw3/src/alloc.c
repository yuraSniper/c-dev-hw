#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "alloc.h"
#include "alloc_internal.h"

static bool initialized = false;
static void * heap;
static free_chunk_header * first_free_chunk; 
static container_descriptor * head[CONTAINER_KIND_COUNT];
static size_t alloc_size_threshold[CONTAINER_KIND_COUNT] =
{
	CONTAINER_SMALL_ITEM_SIZE,
	CONTAINER_MEDIUM_ITEM_SIZE,
	CONTAINER_BIG_ITEM_SIZE,

	0,
};

//NOTE(yura): Private functions:

// chunk_alloc() - allocates a chunk of memory from heap
static chunk_header * chunk_alloc(size_t size)
{
	if (first_free_chunk == NULL)
	{
		fprintf(stderr, "newAlloc internal error: Something went horribly wrong\n");
		return NULL;
	}

	//NOTE(yura): Ensure that size is multiple of 8
	size = size + (8 - (size % 8)) % 8;

	size_t best_size = first_free_chunk->length_kind & CHUNK_LENGTH_MASK;
	free_chunk_header * best_chunk = first_free_chunk;

	while (true)
	{
		free_chunk_header * tmp = best_chunk->next_free;

		if (tmp == NULL)
			break;

		size_t length = (tmp->length_kind & CHUNK_LENGTH_MASK);

		if (length - sizeof(chunk_header) >= size && length > best_size)
		{
			best_chunk = tmp;
			best_size = length;
		}
	}

	if (best_size - sizeof(chunk_header) < size)
	{
		return NULL;
	}

	free_chunk_header * new_free_chunk = best_chunk + size + sizeof(chunk_header);
	new_free_chunk->prev_ptr = best_chunk;
	new_free_chunk->length_flags = (best_size - size - sizeof(chunk_header)) & CHUNK_LENGTH_MASK;
	new_free_chunk->next_free = best_chunk->next_free;
	new_free_chunk->prev_free = best_chunk->prev_free;
	
	chunk_header * next_chunk = best_chunk + best_size;

	if (next_chunk < (heap + INITIAL_HEAP_SIZE))
	{
		next_chunk->prev_ptr = new_free_chunk;
	}

	if (best_chunk->prev_free != NULL)
	{
		best_chunk->prev_free->next_free = new_free_chunk;
	}
	else
	{
		first_free_chunk = new_free_chunk;
	}

	if (best_chunk->next_free != NULL)
	{
		best_chunk->next_free->prev_free = new_free_chunk;
	}

	best_chunk->length_flags = (size + sizeof(chunk_header)) & CHUNK_LENGTH_MASK;

	return ((chunk_header *) best_chunk);
}

// chunk_dealloc() - deallocates a chunk back into the heap
static void chunk_dealloc(chunk_header * ptr)
{
	//NOTE(yura): Algorithm:
	// * try merge with the next chunk
	// * try to merge with prev chunk

	free_chunk_header * current = (free_chunk_header *) ptr;
	current->length_flags &= CHUNK_LENGTH_MASK;

	chunk_header * next_chunk = ((uint8_t *)ptr + (ptr->length_flags & CHUNK_LENGTH_MASK));

	if (next_chunk < (heap + INITIAL_HEAP_SIZE) (next_chunk->length_flags & CHUNK_FLAG_OCCUPIED) == 0)
	{
		free_chunk_header * tmp = (free_chunk_header *) next_chunk;

		current->length_kind += tmp->length_kind & CHUNK_LENGTH_MASK;
		current->next_free = tmp->next_free;
		current->prev_free = tmp->prev_free;

		if (tmp->prev_free != NULL)
			tmp->prev_free->next_free = current;

		if (tmp->next_free != NULL)
			tmp->next_free->prev_free = current;
	}

	if (current->prev_ptr != NULL && (current->prev_ptr->length_flags & CHUNK_FLAG_OCCUPIED) == 0)
	{
		free_chunk_header * tmp = (free_chunk_header *) current->prev_ptr;

		tmp->length_kind += current->length_kind & CHUNK_LENGTH_MASK;
	}
}

// item_alloc() - allocates an item from specialized container
static void * item_alloc(container_kind kind)
{
	container_descriptor * container = head[kind]; 
	
	while (get_free_word_index(container) == (uint32_t) -1)
	{
		container = container->next;

		if (container == head[kind])
		{
			container = container_alloc(kind);
			break;
		}
	}
	
	uint32_t word_index = get_free_word_index(container);
	uint32_t item_index = 0;

	for (uint32_t bit_index; bit_index < 32; bit_index++)
	{
		uint32_t bit_mask = (1 << bit_index);

		if (container->bitmap[word_index] & bit_mask == 0)
		{
			container->bitmap[word_index] |= bit_mask;
			item_index = word_index * 32 + bit_index;

			return container->container_start + item_index * alloc_size_threshold[kind];
		}
	}

	return NULL;
}

// item_dealloc() - frees the item from specific container
static void item_dealloc(container_descriptor * desc, uint32_t index)
{
	uint32_t word_index = index / 32;
	uint32_t bit_index = index % 32;

	desc->bitmap[word_index] &= ~(1 << bit_index);
}

// get_free_word_index() - returns index of a word that has a free slot
static uint32_t get_free_word_index(container_descriptor * desc)
{
	uint32_t kind = desc->length_kind & CONTAINER_KIND_MASK;
	size_t length = desc->length_kind & CONTAINER_LENGTH_MASK;

	uint32_t bitmap_word_count = length / alloc_size_threshold[kind];
	bitmap_word_count /= 32;

	for (uint32_t bitmap_index = 0; bitmap_index < bitmap_word_count; bitmap_index++)
	{
		if (desc->bitmap[bitmap_index] != (uint32_t) -1)
		{
			return bitmap_index;
		}
	}

	return -1;
}

// container_alloc() - allocates a special container with a descriptor for it
static container_descriptor * container_alloc(container_kind kind)
{
	//NOTE(yura): alloc descriptor struct and alloc the container

	//NOTE(yura): We use bitmap, so we have 8 items per byte
	size_t bitmap_size = CONTAINER_SIZE / (alloc_size_threshold[kind] * 8);
	
	size_t desc_size = sizeof(container_descriptor) + bitmap_size;

	container_descriptor * desc = chunk_alloc(desc_size);
	void * container = chunk_alloc(CONTAINER_SIZE); 

	desc->container_start = container;
	desc->length_kind = (CONTAINER_SIZE & CONTAINER_LENGTH_MASK) | (kind);
	
	memset(desc->bitmap, 0, bitmap_size);

	return desc;
}

//NOTE(yura): Public interface functions:

bool mstart()
{
	//NOTE(yura): The overview of the algorithm for this procedure:
	// * allocate big heap
	// * allocate and initialize small containers
	// * set initialized to true

	heap = malloc(INITIAL_GENERIC_CONTAINER_SIZE);

	if (heap == NULL)
	{
		fprintf(stderr, "newAlloc internal error: Cannot preallocate the required space (%u) for initial generic container\n", INITIAL_GENERIC_CONTAINER_SIZE);
		return false;
	}

	free_chunk_header * chunk = (chunk_header *) heap;
	chunk->length_flags = (sizeof(container_descriptor) & CHUNK_LENGTH_MASK);
	chunk->next_free = NULL;
	chunk->prev_free = NULL;

	//NOTE(yura): Special container allocation:

	for (int index = CONTAINER_SMALL; index < CONTAINER_KIND_COUNT; index++)
	{
		if (alloc_size_threshold[index] > 0)
		{
			container_descriptor * cont = container_alloc(index);
			cont->prev = NULL;
			cont->next = NULL;

			head[index] = cont;
		}
	}

	initialized = true;

	return true;
}

void mstop()
{
	if (!initialized)
	{
		fprintf(stderr, "newAlloc internal error: Attempt to deinit a non initialized allocator\n");
		return;
	}
	
	initialized = false;

	free(head[CONTAINER_GENERIC]->container_start);
}

void * alloc(uint32_t size)
{
	//NOTE(yura): Algorithm overview:
	// * decide which container kind we should allocate from
	// * find a container that can hold the chunk of requested size

	container_kind kind = CONTAINER_SMALL;

	while (size > alloc_size_threshold[kind])
	{
		kind++;
	}

	

	return NULL;
}

void mfree(void * ptr)
{
	//NOTE(yura): Algorithm:
	// * find which container ptr belongs to
	// * deallocate it
}
