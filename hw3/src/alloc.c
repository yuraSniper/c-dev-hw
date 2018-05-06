#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "alloc.h"
#include "alloc_internal.h"

static bool initialized = false;
static container_descriptor * head[CONTAINER_KIND_COUNT];
static size_t alloc_size_threshold[CONTAINER_KIND_COUNT] =
{
	CONTAINER_SMALL_ITEM_SIZE,
	CONTAINER_MEDIUM_ITEM_SIZE,
	CONTAINER_BIG_ITEM_SIZE,

	0,
	0,
	0,
	0,

	-1,
};

//NOTE(yura): Private functions:

// chunk_alloc() - allocates a chunk of memory from specific container
// This function requires at least a single container of respective kind to be initialized
static void * chunk_alloc(container_kind kind, size_t size)
{
	return NULL;
}

// container_alloc() - allocates a special container with a descriptor for it
// This function requires at least a single generic container to allocate from
static container_descriptor * container_alloc(container_kind kind)
{
	//NOTE(yura): alloc descriptor struct and alloc the container

	return NULL;
}

//NOTE(yura): Public interface functions:

bool mstart()
{
	//NOTE(yura): The overview of the algorithm for this procedure:
	// * allocate big generic container
	// * initialize container_descriptor for the initial container
	// * allocate small containers
	// * set initialized to true

	void * heap = malloc(INITIAL_GENERIC_CONTAINER_SIZE);

	if (heap == NULL)
	{
		fprintf(stderr, "newAlloc internal error: Cannot preallocate the required space (%u) for initial generic container\n", INITIAL_GENERIC_CONTAINER_SIZE);
		return false;
	}

	chunk_header * chunk = (chunk_header *) heap;
	chunk->length_flags = (sizeof(container_descriptor) & CHUNK_FLAGS_MASK) |
	                      (CHUNK_FLAG_DESCRIPTOR);

	container_descriptor * desc = (container_descriptor *) chunk->data;
	desc->container_start = heap;
	desc->length_kind = (INITIAL_GENERIC_CONTAINER_SIZE	& CONTAINER_LENGTH_MASK) |
	                    (CONTAINER_GENERIC);
	desc->next = desc;
	desc->prev = desc;

	head[CONTAINER_GENERIC] = desc;
	chunk->owner_container = desc;

	//NOTE(yura): Special container allocation:
	
	for (int index = CONTAINER_SMALL; index < CONTAINER_GENERIC; index++)
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
