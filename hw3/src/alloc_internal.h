#ifndef ALLOC_INTERNAL_H
#define ALLOC_INTERNAL_H

#include <stdint.h>
#include <stdbool.h>

#define KB(x) (  (x) * 1024)
#define MB(x) (KB(x) * 1024)
#define GB(x) (MB(x) * 1024)

#define INITIAL_HEAP_SIZE (MB(512))
#define MEM_PAGE_SIZE                  (4096)
#define CONTAINER_KIND_MASK            ((size_t) 0x07)
#define CONTAINER_LENGTH_MASK          (~CONTAINER_KIND_MASK)
#define CHUNK_FLAGS_MASK               ((size_t) 0x07)
#define CHUNK_LENGTH_MASK              (~CHUNK_FLAGS_MASK)

#define CONTAINER_SIZE                 (MEM_PAGE_SIZE * 2)
#define CONTAINER_SMALL_ITEM_SIZE      ( 8)
#define CONTAINER_MEDIUM_ITEM_SIZE     (32)
#define CONTAINER_BIG_ITEM_SIZE        (64)

typedef enum container_kind
{
	CONTAINER_SMALL,
	CONTAINER_MEDIUM,
	CONTAINER_BIG,

	CONTAINER_RESERVED1,
	CONTAINER_RESERVED2,
	CONTAINER_RESERVED3,
	CONTAINER_RESERVED4,

	CONTAINER_KIND_COUNT,
} container_kind;

typedef enum chunk_flags
{
	CHUNK_FLAG_FREE = 0x00,
	CHUNK_FLAG_OCCUPIED = 0x01,
	CHUNK_FLAG_DESCRIPTOR = 0x02,

	CHUNK_FLAG_UNUSED = 0x04,
} chunk_flags;

struct container_descriptor;
struct chunk_header;
struct free_chunk_header;

typedef struct container_descriptor
{
	void * container_start;
	size_t length_kind; //NOTE(yura): Use CONTAINER_*_MASK to extract length or kind

	struct container_descriptor * next;
	struct container_descriptor * prev;

	struct free_chunk_header * first_free_chunk;

	uint32_t bitmap[];
} container_descriptor;

typedef struct chunk_header
{
	size_t length_flags; //NOTE(yura): Use CHUNK_*_MASK to extract length or flags

	uint8_t data[];
} chunk_header;

typedef struct free_chunk_header
{
	size_t length_flags;
	container_descriptor * owner_container;

	struct chunk_header * next_free;
	struct chunk_header * prev_free;
} free_chunk_header;

static void * chunk_alloc(container_kind kind, size_t size);
static container_descriptor * container_alloc(container_kind kind);

#endif //ALLOC_INTERNAL_H
