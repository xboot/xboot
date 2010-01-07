/*
 * lib/libc/malloc.c
 */

#include <configs.h>
#include <default.h>
#include <types.h>
#include <string.h>
#include <malloc.h>

/*
 * internal variable used by brk/sbrk
 */
static x_s8 heap[CONFIG_HEAP_SIZE] __attribute__((used, __section__(".heap")));
static x_s8 * __current_brk = &heap[0];

/*
 * p is an address,  a is alignment; must be a power of 2
 */
static inline void *align_up(void *p, x_u32 a)
{
	return (void *)(((x_u32) p + a - 1) & ~(a - 1));
}

static void *__brk(void * end)
{
	if (end == NULL)
		return &heap[0];

	if( ((x_u32)end < (x_u32)heap) || ((x_u32)end >= CONFIG_HEAP_SIZE + (x_u32)heap) )
		return (void *)-1;

	return &heap[((x_u32)end-(x_u32)heap)];
}

static void *sbrk(x_u32 increment)
{
	x_s8 *start, *end, *new_brk;

	if (!__current_brk)
		__current_brk = __brk(NULL);

	start = align_up(__current_brk, 32);
	end = start + increment;

	new_brk = __brk(end);

	if (new_brk == (void *)-1)
		return (void *)-1;
	else if (new_brk < end)
		return (void *)-1;

	__current_brk = new_brk;

	return start;
}

x_s32 brk(void *end_data_segment)
{
	x_s8 *new_brk;

	new_brk = __brk(end_data_segment);
	if (new_brk != end_data_segment)
		return -1;
	__current_brk = new_brk;
	return 0;
}
/*
 * this structure should be a power of two.  This becomes the
 * alignment unit.
 */
struct free_arena_header;

struct arena_header {
	x_u32 type;
	x_u32 size;
	struct free_arena_header *next, *prev;
};

#define ARENA_TYPE_USED 	0
#define ARENA_TYPE_FREE 	1
#define ARENA_TYPE_HEAD 	2

/*
 * this is the minimum chunk size we will ask the kernel for; this should
 * be a multiple of the page size on all architectures.
 */
#define MALLOC_CHUNK_MASK 	(CONFIG_MALLOC_CHUNK_SIZE - 1)
#define ARENA_SIZE_MASK 	(~(sizeof(struct arena_header)-1))

/*
 * this structure should be no more than twice the size of the
 * previous structure.
 */
struct free_arena_header {
	struct arena_header a;
	struct free_arena_header *next_free, *prev_free;
};

/*
 * both the arena list and the free memory list are double linked
 * list with head node.  this the head node. note that the arena list
 * is sorted in order of address.
 */
static struct free_arena_header __malloc_head = {
	{
		ARENA_TYPE_HEAD,
		0,
		&__malloc_head,
		&__malloc_head,
	},
	&__malloc_head,
	&__malloc_head
};

static inline void remove_from_main_chain(struct free_arena_header *ah)
{
	struct free_arena_header *ap, *an;

	ap = ah->a.prev;
	an = ah->a.next;
	ap->a.next = an;
	an->a.prev = ap;
}

static inline void remove_from_free_chain(struct free_arena_header *ah)
{
	struct free_arena_header *ap, *an;

	ap = ah->prev_free;
	an = ah->next_free;
	ap->next_free = an;
	an->prev_free = ap;
}

static inline void remove_from_chains(struct free_arena_header *ah)
{
	remove_from_free_chain(ah);
	remove_from_main_chain(ah);
}

static void *__malloc_from_block(struct free_arena_header *fp, x_u32 size)
{
	x_u32 fsize;
	struct free_arena_header *nfp, *na, *fpn, *fpp;

	fsize = fp->a.size;

	/*
	 * we need the 2* to account for the larger requirements of a
	 * free block
	 */
	if (fsize >= size + 2 * sizeof(struct arena_header))
	{
		/* bigger block than required -- split block */
		nfp = (struct free_arena_header *)((x_s8 *)fp + size);
		na = fp->a.next;

		nfp->a.type = ARENA_TYPE_FREE;
		nfp->a.size = fsize - size;
		fp->a.type = ARENA_TYPE_USED;
		fp->a.size = size;

		/* insert into all-block chain */
		nfp->a.prev = fp;
		nfp->a.next = na;
		na->a.prev = nfp;
		fp->a.next = nfp;

		/* replace current block on free chain */
		nfp->next_free = fpn = fp->next_free;
		nfp->prev_free = fpp = fp->prev_free;
		fpn->prev_free = nfp;
		fpp->next_free = nfp;
	}
	else
	{
		fp->a.type = ARENA_TYPE_USED; /* allocate the whole block */
		remove_from_free_chain(fp);
	}

	return (void *)(&fp->a + 1);
}

static struct free_arena_header *__free_block(struct free_arena_header *ah)
{
	struct free_arena_header *pah, *nah;

	pah = ah->a.prev;
	nah = ah->a.next;
	if (pah->a.type == ARENA_TYPE_FREE && (x_s8 *)pah + pah->a.size == (x_s8 *)ah)
	{
		/* coalesce into the previous block */
		pah->a.size += ah->a.size;
		pah->a.next = nah;
		nah->a.prev = pah;

		ah = pah;
		pah = ah->a.prev;
	}
	else
	{
		/* need to add this block to the free chain */
		ah->a.type = ARENA_TYPE_FREE;

		ah->next_free = __malloc_head.next_free;
		ah->prev_free = &__malloc_head;
		__malloc_head.next_free = ah;
		ah->next_free->prev_free = ah;
	}

	/*
	 * in either of the previous cases, we might be able to merge
	 * with the subsequent block...
	 */
	if (nah->a.type == ARENA_TYPE_FREE && (x_s8 *)ah + ah->a.size == (x_s8 *)nah)
	{
		ah->a.size += nah->a.size;

		/* remove the old block from the chains */
		remove_from_chains(nah);
	}

	/* return the block that contains the called block */
	return ah;
}

void * malloc(x_u32 size)
{
	struct free_arena_header *fp;
	struct free_arena_header *pah;
	x_u32 fsize;

	if (size == 0)
		return NULL;

	/* add the obligatory arena header, and round up */
	size = (size + 2 * sizeof(struct arena_header) - 1) & ARENA_SIZE_MASK;

	for (fp = __malloc_head.next_free; fp->a.type != ARENA_TYPE_HEAD; fp = fp->next_free)
	{
		if (fp->a.size >= size)
		{
			/* found fit -- allocate out of this block */
			return __malloc_from_block(fp, size);
		}
	}

	/* nothing found... need to request a block from the kernel */
	fsize = (size + MALLOC_CHUNK_MASK) & ~MALLOC_CHUNK_MASK;

	fp = (struct free_arena_header *)sbrk(fsize);

	if (fp == (struct free_arena_header *)((void *)-1))
	{
		return NULL;
	}

	/*
	 * insert the block into the management chains.  we need to set
	 * up the size and the main block list pointer, the rest of
	 * the work is logically identical to free()
	 */
	fp->a.type = ARENA_TYPE_FREE;
	fp->a.size = fsize;

	/*
	 * we need to insert this into the main block list in the proper
	 * place -- this list is required to be sorted.  Since we most likely
	 * get memory assignments in ascending order, search backwards for
	 * the proper place.
	 */
	for (pah = __malloc_head.a.prev; pah->a.type != ARENA_TYPE_HEAD; pah = pah->a.prev)
	{
		if (pah < fp)
			break;
	}

	/*
	 * now pah points to the node that should be the predecessor of
	 * the new node
	 */
	fp->a.next = pah->a.next;
	fp->a.prev = pah;
	pah->a.next = fp;
	fp->a.next->a.prev = fp;

	/* insert into the free chain and coalesce with adjacent blocks */
	fp = __free_block(fp);

	/* now we can allocate from this block */
	return __malloc_from_block(fp, size);
}

void free(void * ptr)
{
	struct free_arena_header *ah;

	if (!ptr)
		return;

	ah = (struct free_arena_header *)((struct arena_header *)ptr - 1);

	/* merge into adjacent free blocks */
	ah = __free_block(ah);

	/* see if it makes sense to return memory to the system */
	if (ah->a.size >= CONFIG_MALLOC_CHUNK_SIZE && (x_s8 *)ah + ah->a.size == __current_brk)
	{
		remove_from_chains(ah);
		brk(ah);
	}
}

void * realloc(void * ptr, x_u32 size)
{
	struct free_arena_header *ah;
	void *newptr;
	x_u32 oldsize;

	if (!ptr)
		return malloc(size);

	if (size == 0)
	{
		free(ptr);
		return NULL;
	}

	/* add the obligatory arena header, and round up */
	size = (size + 2 * sizeof(struct arena_header) - 1) & ARENA_SIZE_MASK;

	ah = (struct free_arena_header *)((struct arena_header *)ptr - 1);

	if (ah->a.size >= size && size >= (ah->a.size >> 2))
	{
		/* this field is a good size already. */
		return ptr;
	}
	else
	{
		/*
		 * make me a new block.  this is kind of bogus; we should
		 * be checking the following block to see if we can do an
		 * in-place adjustment... fix that later.
		 */

		oldsize = ah->a.size - sizeof(struct arena_header);

		newptr = malloc(size);
		memcpy(newptr, ptr, (size < oldsize) ? size : oldsize);
		free(ptr);

		return newptr;
	}
}

void * calloc(x_u32 num, x_u32 size)
{
	void * ptr;

	size *= num;
	ptr = malloc(size);
	if(ptr)
		memset(ptr, 0, size);

	return(ptr);
}
