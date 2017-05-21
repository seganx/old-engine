#if defined(_WIN32)
#include <Windows.h>
#endif
#include <string.h>

#include "Trace.h"
#include "Memory.h"


//////////////////////////////////////////////////////////////////////////
//	memory management
//////////////////////////////////////////////////////////////////////////
#ifdef _WIN32
__declspec(thread) struct sx_memory_manager* s_manager = null;
#else
static __thread struct sx_memory_manager* s_manager = null;
#endif

SEGAN_INLINE void mem_set_manager(struct sx_memory_manager* manager)
{
    s_manager = manager;
}

SEGAN_INLINE struct sx_memory_manager* mem_get_manager(void)
{
    return s_manager;
}

SEGAN_INLINE void* mem_alloc(const uint size_in_byte)
{
    return s_manager ? s_manager->alloc(s_manager, size_in_byte) : malloc(size_in_byte);
}

SEGAN_INLINE void* mem_realloc(void* p, const uint new_size_in_byte)
{
    if (s_manager)
        return s_manager->realloc(s_manager, p, new_size_in_byte);

    if (!new_size_in_byte)
        return mem_free(p);

    void* res = realloc(p, new_size_in_byte);
    if (!res)
    {
        res = p;
        printf("WARNING: Can't reallocate memory!\n");

#ifdef _WIN32
        if (p) { //  if reallocate function failed then try to allocate new one and copy last data to new pool
            newptr = malloc(new_size_in_byte);
            memcpy(newptr, p, mem_min_size(_msize(p), new_size_in_byte));
            free(p);
        }
        else newptr = malloc(new_size_in_byte);
#endif
    }

    return res;
}

SEGAN_INLINE void* mem_free(const void* p)
{
    if (s_manager)
        return s_manager->free(s_manager, p);
    else
        free((void*)p);
    return null;
}

SEGAN_INLINE void mem_copy(void* dest, const void* src, const uint size)
{
    memcpy(dest, src, size);
}

SEGAN_LIB_API sint mem_cmp(const void* src1, const void* src2, const uint size)
{
    return memcmp(src1, src2, size);
}

SEGAN_INLINE void mem_set(void* dest, const sint val, const uint size)
{
    memset(dest, val, size);
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//	MEMORY MANAGER : FREE LIST POOL
//	a fast general memory pool which allocates a memory block in 
//	initialization from OS and uses the allocated 
//	memory pool in any allocation call. using this memory manager has 
//	restriction of block protection.
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

typedef enum
{
    CS_NULL = 0xf0f0f0f0,
    CS_EMPTY = 0xfefefefe,
    CS_FULL = 0xfafafafa
}
memory_chunk_state;

typedef struct memory_chunk
{
    uint		            size;
    memory_chunk_state	    state;
    struct memory_chunk*	behind;
    struct memory_chunk*	next;
    struct memory_chunk*	prev;
}
memory_chunk;

typedef struct memory_pool
{
    struct memory_chunk*    root;
    byte*                   data;
    uint                    size;
}
memory_pool;


static SEGAN_INLINE void memory_pool_push(struct memory_pool* pool, struct memory_chunk* ch)
{
    ch->next = pool->root;
    ch->prev = null;
    pool->root->prev = ch;
    pool->root = ch;
}

static SEGAN_INLINE void memory_pool_pop(struct memory_pool* pool, struct memory_chunk* ch)
{
    if (ch->prev)           ch->prev->next = ch->next;
    if (ch->next)           ch->next->prev = ch->prev;
    if (ch == pool->root)   pool->root = ch->next;
}

static SEGAN_INLINE uint memory_pool_size(const void* p)
{
    if (!p) return 0;
    struct memory_chunk* ch = (struct memory_chunk*)((byte*)(p) - sizeof(struct memory_chunk));
    return ch->size;
}

static SEGAN_INLINE void* memory_pool_alloc( struct sx_memory_manager* manager, const uint sizeinbyte )
{
    struct memory_pool* pool = (struct memory_pool*)((byte*)manager + sizeof(struct sx_memory_manager));
    struct memory_chunk* ch = pool->root;
    while (ch->state == CS_EMPTY)
    {
        if (ch->size >= sizeinbyte)
        {
            //  free chunk founded
            memory_pool_pop(pool, ch);
            if ((ch->size - sizeinbyte) > sizeof(struct memory_chunk))
            {
                //  create new empty chunk by remaining size
                struct memory_chunk* ech = (struct memory_chunk*)((byte*)ch + sizeof(struct memory_chunk) + sizeinbyte);
                ech->behind = ch;
                ech->size = ch->size - (sizeof(struct memory_chunk) + sizeinbyte);
                ech->state = CS_EMPTY;
                memory_pool_push(pool, ech);
                ((struct memory_chunk*)((byte*)ech + sizeof(struct memory_chunk) + ech->size))->behind = ech;
                ch->size = sizeinbyte;
            }
            ch->state = CS_FULL;
            return ((byte*)ch + sizeof(struct memory_chunk));
        }
        ch = ch->next;
    }
    sx_assert("memory_pool_alloc(...) failed due to there is no free chunk exist whit specified size!" == null);
    return null;
}

static SEGAN_INLINE void* memory_pool_free( struct sx_memory_manager* manager, const void* p )
{
    if (!p) return null;
    struct memory_pool* pool = (struct memory_pool*)((byte*)manager + sizeof(struct sx_memory_manager));
    sx_assert((byte*)p > pool->data && (byte*)p < (pool->data + pool->size));

    struct memory_chunk* ch = (struct memory_chunk*)((byte*)p - sizeof(struct memory_chunk));
    sx_assert(ch->state == CS_FULL);	//	avoid to free a chunk more that once

#ifndef NDEBUG
    mem_set((void*)p, 0, ch->size);
#endif

    //  look at neighbor chunk at right side
    struct memory_chunk* rch = (struct memory_chunk*)((byte*)p + ch->size);
    if (rch->state == CS_EMPTY)
    {
        ((struct memory_chunk*)((byte*)rch + sizeof(struct memory_chunk) + rch->size))->behind = ch;
        ch->size += sizeof(struct memory_chunk) + rch->size;
        memory_pool_pop(pool, rch);

#ifndef NDEBUG
        mem_set(rch, 0, sizeof(struct memory_chunk));
#endif
    }

    //  look at neighbor chunk at left side
    if (ch->behind)
    {
        if (ch->behind->state == CS_EMPTY)
        {
            ((struct memory_chunk*)((byte*)(ch) + sizeof(struct memory_chunk) + ch->size))->behind = ch->behind;
            ch->behind->size += sizeof(struct memory_chunk) + ch->size;
#ifndef NDEBUG
            mem_set(ch, 0, sizeof(struct memory_chunk));
#endif
            return null;	//	this one is already exist in list
        }
    }

    //	push to free list
    ch->state = CS_EMPTY;
    memory_pool_push(pool, ch);
    return null;
}

static SEGAN_INLINE void* memory_pool_realloc(struct sx_memory_manager* manager, const void* p, const uint sizeinbyte)
{
#define mem_min_size(a,b) (((a)<(b))?(a):(b))

    if (!p)
    {
        return memory_pool_alloc(manager, sizeinbyte);
    }
    else
    {
        struct memory_pool* pool = (struct memory_pool*)((byte*)manager + sizeof(struct sx_memory_manager));
        sx_assert((byte*)p > pool->data && (byte*)p < (pool->data + pool->size));
        void* tmp = memory_pool_alloc(manager, sizeinbyte);
        uint cursize = memory_pool_size(p);
        mem_copy(tmp, p, mem_min_size(cursize, sizeinbyte));
        memory_pool_free(manager, p);
        return tmp;
    }
}

SEGAN_LIB_API struct sx_memory_manager * sx_mem_pool_create(uint sizeinbyte)
{
    uint memsize = sizeof(struct sx_memory_manager) + sizeof(struct memory_pool) + sizeinbyte + 0x0fff * sizeof(struct memory_chunk);
    byte* mem = (byte*)mem_alloc(memsize);
    sx_assert(mem);
#ifndef NDEBUG
    mem_set(mem, 0, memsize);
#endif

    struct memory_pool* pool = (struct memory_pool*)(mem + sizeof(struct sx_memory_manager));
    pool->data = mem + sizeof(struct sx_memory_manager) + sizeof(struct memory_pool);
    pool->size = memsize;

    //  set first empty chunk
    struct memory_chunk* ch = (struct memory_chunk*)(pool->data);
    ch->state = CS_EMPTY;
    ch->size = memsize - 2 * sizeof(struct memory_chunk);

    //  create last inactive chuck
    struct memory_chunk* ich = (struct memory_chunk*)(mem + memsize - sizeof(struct memory_chunk));
    ich->behind = ch;
    ich->state = CS_NULL;
    ich->size = 0;
    pool->root = ich;
    memory_pool_push(pool, ch);

    //  prepare the instance of memory manager
    struct sx_memory_manager* res = (struct sx_memory_manager*)mem;
    res->alloc = memory_pool_alloc;
    res->realloc = memory_pool_realloc;
    res->free = memory_pool_free;
    return res;
}

SEGAN_LIB_API int sx_mem_pool_destroy(struct sx_memory_manager * mempool)
{
    mem_free(mempool);
    return 0;
}

