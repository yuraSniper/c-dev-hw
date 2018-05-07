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
static void * chunk_alloc(size_t size)
{
	return NULL;
}

// chunk_dealloc() - deallocates a chunk back into the heap
static void chunk_dealloc(void * ptr)
{
	
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

// get_free_word_index() - checks if the container has free slots
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

	chunk_header * chunk = (chunk_header *) heap;
	chunk->length_flags = (sizeof(container_descriptor) & CHUNK_LENGTH_MASK) | (CHUNK_FLAG_FREE);

	//NOTE(yura): Special container allocation:
	
	for (int index = CONTAINER_SMALL; index < CONTAINER_KIND_COUNT; index++)
	{
		if (alloc_size_threshold[index] > 0)
		{
			container_descriptor * cont = container_alloc(index);
			cont->prev = cont;
			cont->next = cont;

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
	return NULL;
}

void mfree(void * ptr)
{

}
