#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "alloc.h"
#include "alloc_internal.h"

static bool initialized = false;
static container_descriptor * head[CONTAINER_KIND_COUNT];
static container_descriptor * tail[CONTAINER_KIND_COUNT];
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
// This function requires single container of respective kind to be initialized
static void * chunk_alloc(container_kind kind, size_t size)
{
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

	void * generic_container = malloc(INITIAL_GENERIC_CONTAINER_SIZE);

	if (generic_container == NULL)
	{
		fprintf(stderr, "newAlloc internal error: Cannot preallocate the required space (%u) for initial generic container\n", INITIAL_GENERIC_CONTAINER_SIZE);
		return false;
	}

	chunk_header * chunk = (chunk_header *) generic_container;
	chunk->length_flags = (sizeof(container_descriptor) & CHUNK_FLAGS_MASK) |
	                      (CHUNK_FLAG_DESCRIPTOR);

	container_descriptor * desc = (container_descriptor *) chunk->data;
	desc->container_start = generic_container;
	desc->length_kind = (INITIAL_GENERIC_CONTAINER_SIZE	& CONTAINER_LENGTH_MASK) |
	                    (CONTAINER_GENERIC);
	desc->next = NULL;
	desc->prev = NULL;

	head[CONTAINER_GENERIC] = desc;
	chunk->owner_container = desc;

	return true;
}

void mstop()
{
	initialized = false;
	//dealloc small containers back-to-front
}

void * alloc(uint32_t size)
{
	return NULL;
}

void mfree(void * ptr)
{

}
