#include "mem_pool.h"
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
extern mem_pool_t *DEFAULT_POOL;

mem_pool_t *
mem_create_pool (size_t size)
{
  mem_pool_t *new_pool;

  if (size < PAGE_SIZE || size % PAGE_SIZE != 0)
    {
      size = PAGE_SIZE;
    }
  int re = posix_memalign ((void **) &new_pool, MEM_ALIGNMENT, size);
  assert (!re);
  new_pool->large = NULL;
  new_pool->current = new_pool->head = (void *) new_pool + sizeof (mem_pool_t);
  new_pool->head->last
    = (void *) new_pool + sizeof (mem_pool_t) + sizeof (mem_node_t);
  new_pool->head->end = (unsigned char *) new_pool + PAGE_SIZE;
  new_pool->head->failed = 0;

  return new_pool;
}

void
mem_destroy_pool (mem_pool_t *pool)
{
  mem_large_t *large;
  for (large = pool->large; large; large = large->next)
    {
      if (large->alloc)
	{
	  free (large->alloc);
	}
    }

  mem_node_t *cur, *next;
  cur = pool->head->next;

  while (cur)
    {
      next = cur->next;
      free (cur);
      cur = next;
    }
  free (pool);
}

void *
_mem_malloc_large (mem_pool_t *pool, size_t size)
{
  unsigned char *big_addr;
  int re = posix_memalign ((void **) &big_addr, MEM_ALIGNMENT, size); // size
  if (re)
    {
      return NULL;
    }

  mem_large_t *large;
  int n = 0;
  for (large = pool->large; large; large = large->next)
    {
      if (large->alloc == NULL)
	{
	  large->size = size;
	  large->alloc = big_addr;
	  return big_addr;
	}
      if (n++ > 3)
	{
	  break;
	}
    }
  large = mem_malloc (pool, sizeof (mem_large_t));
  if (large == NULL)
    {
      free (big_addr);
      return NULL;
    }
  large->size = size;
  large->alloc = big_addr;
  large->next = pool->large;
  pool->large = large;
  return big_addr;
}

void *
_mem_malloc_block (mem_pool_t *pool, size_t size)
{
  unsigned char *block;
  int ret = posix_memalign ((void **) &block, MEM_ALIGNMENT, PAGE_SIZE); // 4K
  if (ret)
    {
      return NULL;
    }
  mem_node_t *new_node = (mem_node_t *) block;
  new_node->end = block + PAGE_SIZE;
  new_node->next = NULL;

  unsigned char *ret_addr
    = mem_align_ptr (block + sizeof (mem_node_t), MEM_ALIGNMENT);

  new_node->last = ret_addr + size;
  new_node->quote++;

  mem_node_t *current = pool->current;
  mem_node_t *cur = NULL;

  for (cur = current; cur->next; cur = cur->next)
    {
      if (cur->failed++ > 4)
	{
	  current = cur->next;
	}
    }
  cur->next = new_node;
  pool->current = current;
  return ret_addr;
}

void *
mem_malloc (mem_pool_t *pool, size_t size)
{
  assert (size > 0);
  if (size > PAGE_SIZE - sizeof (mem_node_t))
    {
      return _mem_malloc_large (pool, size);
    }
  unsigned char *mem_addr = NULL;
  mem_node_t *cur = NULL;
  cur = pool->current;
  while (cur)
    {
      mem_addr = mem_align_ptr (cur->last, MEM_ALIGNMENT);
      if (cur->end - mem_addr >= (long) size)
	{
	  cur->quote++;
	  cur->last = mem_addr + size;
	  return mem_addr;
	}
      else
	{
	  cur = cur->next;
	}
      return _mem_malloc_block (pool, size);
    }
  return 0;
}

void *
mem_calloc (mem_pool_t *pool, size_t size)
{
  void *mem_addr = mem_malloc (pool, size);
  assert (mem_addr);

  memset (mem_addr, 0, size);
  return mem_addr;
}

void
mem_free (mem_pool_t *pool, void *p)
{
  mem_large_t *lp;
  for (lp = pool->large; lp; lp = lp->next)
    {
      if (p == lp->alloc)
	{
	  free (lp->alloc);
	  lp->size = 0;
	  lp->alloc = NULL;
	  return;
	}
    }

  mem_node_t *cur = NULL;
  for (cur = pool->head; cur; cur = cur->next)
    {
      if ((unsigned char *) cur <= (unsigned char *) p
	  && (unsigned char *) p <= (unsigned char *) cur->end)
	{
	  cur->quote--;
	  if (cur->quote == 0)
	    {
	      if (cur == pool->head)
		{
		  pool->head->last = (unsigned char *) pool
				     + sizeof (mem_pool_t)
				     + sizeof (mem_node_t);
		}
	      else
		{
		  cur->last = (unsigned char *) cur + sizeof (mem_node_t);
		}
	      cur->failed = 0;
	      pool->current = pool->head;
	    }
	  return;
	}
    }
}

void *
mem_yy_malloc (size_t size)
{
  return mem_malloc (DEFAULT_POOL, size);
}

void
mem_yy_free (void *p)
{
  return mem_free (DEFAULT_POOL, p);
}