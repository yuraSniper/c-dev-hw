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
