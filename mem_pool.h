#ifndef MEM_POOL_H
#define MEM_POOL_H

// similar to nginx's memory pool

#include "config.h"

#define PAGE_SIZE 4096
#define MEM_ALIGNMENT 16
#define mem_align(n, alignment) (((n) + (alignment - 1)) & ~(alignment - 1))
#define mem_align_ptr(p, alignment)                                            \
  (void *) ((((size_t) p) + (alignment - 1)) & ~(alignment - 1))

typedef struct mp_node_s
{
  unsigned char *end;
  unsigned char *last;
  int quote;
  int failed;
  struct mp_node_s *next;
} mem_node_t;

typedef struct mem_large_s
{ // for large chunk require
  size_t size;
  void *alloc;
  struct mem_large_s *next;
} mem_large_t;

typedef struct mem_pool_s
{
  mem_large_t *large;
  mem_node_t *head;
  mem_node_t *current; // if there is no enough space, point to next block
} mem_pool_t;

void *
mem_malloc (mem_pool_t *pool, size_t size);
void
mem_free (mem_pool_t *pool, void *p);
void *
mem_calloc (mem_pool_t *pool, size_t size);
mem_pool_t *
mem_create_pool (size_t size);
void
mem_destroy_pool (mem_pool_t *pool);
#endif