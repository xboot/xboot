/*
 * lib/libc/malloc/malloc.c
 */

#include <xboot.h>
#include <xboot/module.h>
#include <runtime.h>
#include <malloc.h>

/*
 * Some macros.
 */
#define tlsf_cast(t, exp)			((t) (exp))
#define tlsf_min(a, b)				((a) < (b) ? (a) : (b))
#define tlsf_max(a, b)				((a) > (b) ? (a) : (b))

#define tlsf_assert					assert
#define tlsf_insist(x)				{ tlsf_assert(x); if (!(x)) { status--; } }

/*
 * Public constants
 */
enum tlsf_public
{
	/*
	 * log2 of number of linear subdivisions of block sizes
	 */
	SL_INDEX_COUNT_LOG2 = 5,
};

/*
 * Private constants
 */
enum tlsf_private
{
#if defined (TLSF_64BIT)
	/*
	 * All allocation sizes and addresses are aligned to 8 bytes
	 */
	ALIGN_SIZE_LOG2 = 3,
#else
	/*
	 * All allocation sizes and addresses are aligned to 4 bytes
	 */
	ALIGN_SIZE_LOG2 = 2,
#endif
	ALIGN_SIZE = (1 << ALIGN_SIZE_LOG2),

#if defined (TLSF_64BIT)
	FL_INDEX_MAX = 32,
#else
	FL_INDEX_MAX = 30,
#endif
	SL_INDEX_COUNT = (1 << SL_INDEX_COUNT_LOG2),
	FL_INDEX_SHIFT = (SL_INDEX_COUNT_LOG2 + ALIGN_SIZE_LOG2),
	FL_INDEX_COUNT = (FL_INDEX_MAX - FL_INDEX_SHIFT + 1),

	SMALL_BLOCK_SIZE = (1 << FL_INDEX_SHIFT),
};

/*
 * Block header structure
 */
typedef struct block_header_t
{
	/*
	 * Points to the previous physical block
	 */
	struct block_header_t * prev_phys_block;

	/*
	 * The size of this block, excluding the block header
	 */
	size_t size;

	/*
	 * Next and previous free blocks
	 */
	struct block_header_t * next_free;
	struct block_header_t * prev_free;
} block_header_t;

/*
 * The TLSF pool structure
 */
typedef struct pool_t
{
	/*
	 * Empty lists point at this block to indicate they are free
	 */
	block_header_t block_null;

	/*
	 * Bitmaps for free lists
	 */
	unsigned int fl_bitmap;
	unsigned int sl_bitmap[FL_INDEX_COUNT];

	/*
	 * Head of free lists
	 */
	block_header_t* blocks[FL_INDEX_COUNT][SL_INDEX_COUNT];
} pool_t;

typedef struct integrity_t
{
	int prev_status;
	int status;
} integrity_t;

/*
 * A type used for casting when doing pointer arithmetic
 */
typedef ptrdiff_t tlsfptr_t;

/*
 * Associated constants
 */
static const size_t block_header_free_bit = 1 << 0;
static const size_t block_header_prev_free_bit = 1 << 1;
static const size_t block_header_overhead = sizeof(size_t);
static const size_t block_start_offset = offsetof(block_header_t, size) + sizeof(size_t);
static const size_t block_size_min = sizeof(block_header_t) - sizeof(block_header_t *);
static const size_t block_size_max = tlsf_cast(size_t, 1) << FL_INDEX_MAX;

static int tlsf_fls_generic(unsigned int word)
{
	int bit = 32;

	if (!word) bit -= 1;
	if (!(word & 0xffff0000)) { word <<= 16; bit -= 16; }
	if (!(word & 0xff000000)) { word <<= 8; bit -= 8; }
	if (!(word & 0xf0000000)) { word <<= 4; bit -= 4; }
	if (!(word & 0xc0000000)) { word <<= 2; bit -= 2; }
	if (!(word & 0x80000000)) { word <<= 1; bit -= 1; }

	return bit;
}

static int tlsf_ffs(unsigned int word)
{
	return tlsf_fls_generic(word & (~word + 1)) - 1;
}

static int tlsf_fls(unsigned int word)
{
	return tlsf_fls_generic(word) - 1;
}

#if defined (TLSF_64BIT)
static int tlsf_fls_sizet(size_t size)
{
	int high = (int)(size >> 32);
	int bits = 0;

	if(high)
	{
		bits = 32 + tlsf_fls(high);
	}
	else
	{
		bits = tlsf_fls((int)size & 0xffffffff);

	}

	return bits;
}
#else
#define tlsf_fls_sizet	tlsf_fls
#endif

static size_t block_size(const block_header_t * block)
{
	return block->size & ~(block_header_free_bit | block_header_prev_free_bit);
}

static void block_set_size(block_header_t * block, size_t size)
{
	const size_t oldsize = block->size;
	block->size = size | (oldsize & (block_header_free_bit | block_header_prev_free_bit));
}

static int block_is_last(const block_header_t * block)
{
	return (block_size(block) == 0);
}

static int block_is_free(const block_header_t * block)
{
	return tlsf_cast(int, block->size & block_header_free_bit);
}

static void block_set_free(block_header_t * block)
{
	block->size |= block_header_free_bit;
}

static void block_set_used(block_header_t * block)
{
	block->size &= ~block_header_free_bit;
}

static int block_is_prev_free(const block_header_t * block)
{
	return tlsf_cast(int, block->size & block_header_prev_free_bit);
}

static void block_set_prev_free(block_header_t * block)
{
	block->size |= block_header_prev_free_bit;
}

static void block_set_prev_used(block_header_t * block)
{
	block->size &= ~block_header_prev_free_bit;
}

static block_header_t * block_from_ptr(const void * ptr)
{
	return tlsf_cast(block_header_t *, tlsf_cast(unsigned char*, ptr) - block_start_offset);
}

static void* block_to_ptr(const block_header_t * block)
{
	return tlsf_cast(void *, tlsf_cast(unsigned char *, block) + block_start_offset);
}

static block_header_t * offset_to_block(const void * ptr, size_t size)
{
	return tlsf_cast(block_header_t *, tlsf_cast(tlsfptr_t, ptr) + size);
}

static block_header_t * block_prev(const block_header_t * block)
{
	return block->prev_phys_block;
}

static block_header_t* block_next(const block_header_t* block)
{
	block_header_t * next = offset_to_block(block_to_ptr(block), block_size(block) - block_header_overhead);
	tlsf_assert(!block_is_last(block));

	return next;
}

static block_header_t* block_link_next(block_header_t* block)
{
	block_header_t * next = block_next(block);

	next->prev_phys_block = block;
	return next;
}

static void block_mark_as_free(block_header_t * block)
{
	block_header_t * next = block_link_next(block);

	block_set_prev_free(next);
	block_set_free(block);
}

static void block_mark_as_used(block_header_t * block)
{
	block_header_t * next = block_next(block);

	block_set_prev_used(next);
	block_set_used(block);
}

static size_t align_up(size_t x, size_t align)
{
	tlsf_assert(0 == (align & (align - 1)) && "must align to a power of two");
	return (x + (align - 1)) & ~(align - 1);
}

static size_t align_down(size_t x, size_t align)
{
	tlsf_assert(0 == (align & (align - 1)) && "must align to a power of two");
	return x - (x & (align - 1));
}

static void * align_ptr(const void * ptr, size_t align)
{
	const tlsfptr_t aligned = (tlsf_cast(tlsfptr_t, ptr) + (align - 1)) & ~(align - 1);
	tlsf_assert(0 == (align & (align - 1)) && "must align to a power of two");

	return tlsf_cast(void*, aligned);
}

static size_t adjust_request_size(size_t size, size_t align)
{
	size_t adjust = 0;

	if(size && size < block_size_max)
	{
		const size_t aligned = align_up(size, align);
		adjust = tlsf_max(aligned, block_size_min);
	}

	return adjust;
}

static void mapping_insert(size_t size, int* fli, int* sli)
{
	int fl, sl;

	if(size < SMALL_BLOCK_SIZE)
	{
		fl = 0;
		sl = tlsf_cast(int, size) / (SMALL_BLOCK_SIZE / SL_INDEX_COUNT);
	}
	else
	{
		fl = tlsf_fls_sizet(size);
		sl = tlsf_cast(int, size >> (fl - SL_INDEX_COUNT_LOG2)) ^ (1 << SL_INDEX_COUNT_LOG2);
		fl -= (FL_INDEX_SHIFT - 1);
	}

	*fli = fl;
	*sli = sl;
}

static void mapping_search(size_t size, int * fli, int * sli)
{
	if(size >= (1 << SL_INDEX_COUNT_LOG2))
	{
		const size_t round = (1 << (tlsf_fls_sizet(size) - SL_INDEX_COUNT_LOG2)) - 1;
		size += round;
	}
	mapping_insert(size, fli, sli);
}

static block_header_t * search_suitable_block(pool_t * pool, int * fli, int * sli)
{
	int fl = *fli;
	int sl = *sli;
	unsigned int sl_map = pool->sl_bitmap[fl] & (~0 << sl);

	if(!sl_map)
	{
		const unsigned int fl_map = pool->fl_bitmap & (~0 << (fl + 1));
		if (!fl_map)
		{
			return 0;
		}

		fl = tlsf_ffs(fl_map);
		*fli = fl;
		sl_map = pool->sl_bitmap[fl];
	}
	tlsf_assert(sl_map && "internal error - second level bitmap is null");
	sl = tlsf_ffs(sl_map);
	*sli = sl;

	return pool->blocks[fl][sl];
}

static void remove_free_block(pool_t * pool, block_header_t * block, int fl, int sl)
{
	block_header_t * prev = block->prev_free;
	block_header_t * next = block->next_free;
	tlsf_assert(prev && "prev_free field can not be null");
	tlsf_assert(next && "next_free field can not be null");
	next->prev_free = prev;
	prev->next_free = next;

	if (pool->blocks[fl][sl] == block)
	{
		pool->blocks[fl][sl] = next;

		if (next == &pool->block_null)
		{
			pool->sl_bitmap[fl] &= ~(1 << sl);

			if (!pool->sl_bitmap[fl])
			{
				pool->fl_bitmap &= ~(1 << fl);
			}
		}
	}
}

static void insert_free_block(pool_t * pool, block_header_t * block, int fl, int sl)
{
	block_header_t * current = pool->blocks[fl][sl];

	tlsf_assert(current && "free list cannot have a null entry");
	tlsf_assert(block && "cannot insert a null entry into the free list");
	block->next_free = current;
	block->prev_free = &pool->block_null;
	current->prev_free = block;

	tlsf_assert(block_to_ptr(block) == align_ptr(block_to_ptr(block), ALIGN_SIZE)
		&& "block not aligned properly");

	pool->blocks[fl][sl] = block;
	pool->fl_bitmap |= (1 << fl);
	pool->sl_bitmap[fl] |= (1 << sl);
}

static void block_remove(pool_t * pool, block_header_t * block)
{
	int fl, sl;

	mapping_insert(block_size(block), &fl, &sl);
	remove_free_block(pool, block, fl, sl);
}

static void block_insert(pool_t * pool, block_header_t * block)
{
	int fl, sl;

	mapping_insert(block_size(block), &fl, &sl);
	insert_free_block(pool, block, fl, sl);
}

static int block_can_split(block_header_t * block, size_t size)
{
	return block_size(block) >= sizeof(block_header_t) + size;
}

static block_header_t* block_split(block_header_t * block, size_t size)
{
	block_header_t* remaining = offset_to_block(block_to_ptr(block), size - block_header_overhead);
	const size_t remain_size = block_size(block) - (size + block_header_overhead);

	tlsf_assert(block_to_ptr(remaining) == align_ptr(block_to_ptr(remaining), ALIGN_SIZE)
		&& "remaining block not aligned properly");

	tlsf_assert(block_size(block) == remain_size + size + block_header_overhead);
	block_set_size(remaining, remain_size);
	tlsf_assert(block_size(remaining) >= block_size_min && "block split with invalid size");

	block_set_size(block, size);
	block_mark_as_free(remaining);

	return remaining;
}

static block_header_t * block_absorb(block_header_t * prev, block_header_t * block)
{
	tlsf_assert(!block_is_last(prev) && "previous block can't be last!");
	prev->size += block_size(block) + block_header_overhead;
	block_link_next(prev);

	return prev;
}

static block_header_t * block_merge_prev(pool_t * pool, block_header_t * block)
{
	if (block_is_prev_free(block))
	{
		block_header_t * prev = block_prev(block);
		tlsf_assert(prev && "prev physical block can't be null");
		tlsf_assert(block_is_free(prev) && "prev block is not free though marked as such");
		block_remove(pool, prev);
		block = block_absorb(prev, block);
	}

	return block;
}

static block_header_t * block_merge_next(pool_t * pool, block_header_t * block)
{
	block_header_t* next = block_next(block);
	tlsf_assert(next && "next physical block can't be null");

	if(block_is_free(next))
	{
		tlsf_assert(!block_is_last(block) && "previous block can't be last!");
		block_remove(pool, next);
		block = block_absorb(block, next);
	}

	return block;
}

static void block_trim_free(pool_t* pool, block_header_t* block, size_t size)
{
	tlsf_assert(block_is_free(block) && "block must be free");
	if(block_can_split(block, size))
	{
		block_header_t * remaining_block = block_split(block, size);
		block_link_next(block);
		block_set_prev_free(remaining_block);
		block_insert(pool, remaining_block);
	}
}

static void block_trim_used(pool_t * pool, block_header_t * block, size_t size)
{
	tlsf_assert(!block_is_free(block) && "block must be used");
	if(block_can_split(block, size))
	{
		block_header_t * remaining_block = block_split(block, size);
		block_set_prev_used(remaining_block);

		remaining_block = block_merge_next(pool, remaining_block);
		block_insert(pool, remaining_block);
	}
}

static block_header_t * block_trim_free_leading(pool_t * pool, block_header_t * block, size_t size)
{
	block_header_t * remaining_block = block;

	if(block_can_split(block, size))
	{
		remaining_block = block_split(block, size - block_header_overhead);
		block_set_prev_free(remaining_block);

		block_link_next(block);
		block_insert(pool, block);
	}

	return remaining_block;
}

static block_header_t * block_locate_free(pool_t * pool, size_t size)
{
	int fl = 0, sl = 0;
	block_header_t * block = 0;

	if(size)
	{
		mapping_search(size, &fl, &sl);
		block = search_suitable_block(pool, &fl, &sl);
	}

	if(block)
	{
		tlsf_assert(block_size(block) >= size);
		remove_free_block(pool, block, fl, sl);
	}

	return block;
}

static void * block_prepare_used(pool_t * pool, block_header_t * block, size_t size)
{
	void * p = 0;

	if(block)
	{
		block_trim_free(pool, block, size);
		block_mark_as_used(block);
		p = block_to_ptr(block);
	}

	return p;
}

static void pool_construct(pool_t * pool)
{
	int i, j;

	pool->block_null.next_free = &pool->block_null;
	pool->block_null.prev_free = &pool->block_null;

	pool->fl_bitmap = 0;
	for(i = 0; i < FL_INDEX_COUNT; ++i)
	{
		pool->sl_bitmap[i] = 0;
		for(j = 0; j < SL_INDEX_COUNT; ++j)
		{
			pool->blocks[i][j] = &pool->block_null;
		}
	}
}

static size_t tlsf_overhead()
{
	const size_t pool_overhead = sizeof(pool_t) + 2 * block_header_overhead;
	return pool_overhead;
}

static void * tlsf_create(void * mem, size_t bytes)
{
	block_header_t * block;
	block_header_t * next;

	const size_t pool_overhead = tlsf_overhead();
	const size_t pool_bytes = align_down(bytes - pool_overhead, ALIGN_SIZE);
	pool_t * pool = tlsf_cast(pool_t*, mem);

	if((pool_bytes < block_size_min) || (pool_bytes > block_size_max))
		return 0;

	pool_construct(pool);

	block = offset_to_block(tlsf_cast(void *, pool), sizeof(pool_t) - block_header_overhead);
	block_set_size(block, pool_bytes);
	block_set_free(block);
	block_set_prev_used(block);
	block_insert(pool, block);

	next = block_link_next(block);
	block_set_size(next, 0);
	block_set_used(next);
	block_set_prev_free(next);

	return tlsf_cast(void *, pool);
}

static void tlsf_destroy(void * pool)
{
}

static void * tlsf_malloc(void * tlsf, size_t size)
{
	pool_t * pool = tlsf_cast(pool_t *, tlsf);
	const size_t adjust = adjust_request_size(size, ALIGN_SIZE);
	block_header_t* block = block_locate_free(pool, adjust);

	return block_prepare_used(pool, block, adjust);
}

static void * tlsf_memalign(void * tlsf, size_t align, size_t size)
{
	pool_t * pool = tlsf_cast(pool_t *, tlsf);
	const size_t adjust = adjust_request_size(size, ALIGN_SIZE);
	const size_t gap_minimum = sizeof(block_header_t);
	const size_t size_with_gap = adjust_request_size(adjust + align + gap_minimum, align);
	const size_t aligned_size = (align <= ALIGN_SIZE) ? adjust : size_with_gap;
	block_header_t * block = block_locate_free(pool, aligned_size);

	tlsf_assert(sizeof(block_header_t) == block_size_min + block_header_overhead);

	if(block)
	{
		void * ptr = block_to_ptr(block);
		void * aligned = align_ptr(ptr, align);
		size_t gap = tlsf_cast(size_t, tlsf_cast(tlsfptr_t, aligned) - tlsf_cast(tlsfptr_t, ptr));

		if(gap && gap < gap_minimum)
		{
			const size_t gap_remain = gap_minimum - gap;
			const size_t offset = tlsf_max(gap_remain, align);
			const void * next_aligned = tlsf_cast(void*, tlsf_cast(tlsfptr_t, aligned) + offset);

			aligned = align_ptr(next_aligned, align);
			gap = tlsf_cast(size_t,	tlsf_cast(tlsfptr_t, aligned) - tlsf_cast(tlsfptr_t, ptr));
		}

		if(gap)
		{
			tlsf_assert(gap >= gap_minimum && "gap size too small");
			block = block_trim_free_leading(pool, block, gap);
		}
	}

	return block_prepare_used(pool, block, adjust);
}

static void tlsf_free(void * tlsf, void * ptr)
{
	if(ptr)
	{
		pool_t* pool = tlsf_cast(pool_t*, tlsf);
		block_header_t* block = block_from_ptr(ptr);

		block_mark_as_free(block);
		block = block_merge_prev(pool, block);
		block = block_merge_next(pool, block);
		block_insert(pool, block);
	}
}

static void * tlsf_realloc(void * tlsf, void * ptr, size_t size)
{
	pool_t * pool = tlsf_cast(pool_t *, tlsf);
	void * p = 0;

	if(ptr && size == 0)
	{
		tlsf_free(tlsf, ptr);
	}
	else if(!ptr)
	{
		p = tlsf_malloc(tlsf, size);
	}
	else
	{
		block_header_t * block = block_from_ptr(ptr);
		block_header_t * next = block_next(block);

		const size_t cursize = block_size(block);
		const size_t combined = cursize + block_size(next) + block_header_overhead;
		const size_t adjust = adjust_request_size(size, ALIGN_SIZE);

		if((adjust > cursize) && (!block_is_free(next) || (adjust > combined)))
		{
			p = tlsf_malloc(tlsf, size);
			if(p)
			{
				const size_t minsize = tlsf_min(cursize, size);

				memcpy(p, ptr, minsize);
				tlsf_free(tlsf, ptr);
			}
		}
		else
		{
			if(adjust > cursize)
			{
				block_merge_next(pool, block);
				block_mark_as_used(block);
			}

			block_trim_used(pool, block, adjust);
			p = ptr;
		}
	}

	return p;
}

void * memory_pool_create(void * pool, size_t size)
{
	return tlsf_create(pool, size);
}

void memory_pool_destroy(void * pool)
{
	tlsf_destroy(pool);
}

struct mpool_stat {
	size_t used;
	size_t free;
};

typedef void (*tlsf_walker)(void * ptr, size_t size, int used, void * user);

static void tlfs_stat_walker(void * ptr, size_t size, int used, void * user)
{
	struct mpool_stat * stat = user;

	if(used)
		stat->used += size;
	else
		stat->free += size;
}

static void tlsf_walk_heap(void * pool, tlsf_walker walker, void * user)
{
	block_header_t * block = offset_to_block(pool, sizeof(pool_t) - block_header_overhead);

	if(!pool || !walker)
		return;

	while(block && !block_is_last(block))
	{
		walker(block_to_ptr(block), block_size(block), !block_is_free(block), user);
		block = block_next(block);
	}
}

void memory_pool_stat(void * pool, size_t * used, size_t * free)
{
	struct mpool_stat stat;

	stat.used = 0;
	stat.free = 0;

	tlsf_walk_heap(pool, tlfs_stat_walker, &stat);

	*used = stat.used;
	*free = stat.free;
}

void * malloc(size_t size)
{
	return tlsf_malloc(runtime_get()->__pool, size);
}
EXPORT_SYMBOL(malloc);

void * memalign(size_t align, size_t size)
{
	return tlsf_memalign(runtime_get()->__pool, align, size);
}
EXPORT_SYMBOL(memalign);

void * realloc(void * ptr, size_t size)
{
	return tlsf_realloc(runtime_get()->__pool, ptr, size);
}
EXPORT_SYMBOL(realloc);

void * calloc(size_t nmemb, size_t size)
{
	void * ptr;

	if((ptr = malloc(nmemb * size)))
		memset(ptr, 0, nmemb * size);

	return ptr;
}
EXPORT_SYMBOL(calloc);

void free(void * ptr)
{
	tlsf_free(runtime_get()->__pool, ptr);
}
EXPORT_SYMBOL(free);
