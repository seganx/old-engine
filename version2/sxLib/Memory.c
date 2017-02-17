#if defined(_WIN32)
#include <Windows.h>
#endif
#include <string.h>

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
    return s_manager ? s_manager->alloc(size_in_byte) : malloc(size_in_byte);
}

SEGAN_INLINE void* mem_realloc(void* p, const uint new_size_in_byte)
{
    if (s_manager)
        return s_manager->realloc(p, new_size_in_byte);

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
        return s_manager->free(p);
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
#if 0

#define mem_min_size(a,b) (((a)<(b))?(a):(b))


typedef enum
{
    MCS_NULL = 0xf0f0f0f0,
    MCS_EMPTY = 0xfefefefe,
    MCS_FULL = 0xfafafafa
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

typedef struct sx_memorypool
{
    byte*                   pool;
    struct memory_chunk*    root;
}
sx_memorypool;


//////////////////////////////////////////////////////////////////////////
//	MEMORY MANAGER : FREE LIST POOL
//	a fast general memory pool which allocates a memory block in 
//	initialization from OS and uses the allocated 
//	memory pool in any allocation call. using this memory manager has 
//	restriction of block protection.
//////////////////////////////////////////////////////////////////////////
MemMan_Pool::MemMan_Pool(const uint poolSizeInBytes) : MemMan()
{
    uint memsize = poolSizeInBytes + 0x0fff * sizeof(Chunk);
    m_pool = (pbyte)mem_alloc(memsize);
    sx_assert(m_pool);
#ifdef _DEBUG
    mem_set(m_pool, 0, memsize);
#endif
    //  set first empty chunk
    Chunk* ch = PChunk(m_pool);
    ch->state = CS_EMPTY;
    ch->size = memsize - 2 * sizeof(Chunk);

    //  create last inactive chuck
    Chunk*	ich = PChunk(m_pool + memsize - sizeof(Chunk));
    ich->behind = ch;
    ich->state = CS_NULL;
    ich->size = 0;
    m_root = ich;
    push(ch);
}

MemMan_Pool::~MemMan_Pool(void)
{
    mem_free(m_pool);
}

void* MemMan_Pool::alloc(const uint sizeInByte)
{
    Chunk* ch = m_root;
    while (ch->state == CS_EMPTY)
    {
        if (ch->size >= sizeInByte)
        {
            //  free chunk founded
            pop(ch);
            if ((ch->size - sizeInByte) > sizeof(Chunk))
            {
                //  create new empty chunk by remaining size
                Chunk* ech = PChunk(pbyte(ch) + sizeof(Chunk) + sizeInByte);
                ech->behind = ch;
                ech->size = ch->size - (sizeof(Chunk) + sizeInByte);
                ech->state = CS_EMPTY;
                push(ech);
                PChunk(pbyte(ech) + sizeof(Chunk) + ech->size)->behind = ech;
                ch->size = sizeInByte;
            }
            ch->state = CS_FULL;
            return (pbyte(ch) + sizeof(Chunk));
        }
        ch = ch->next;
    }
    sx_assert("MemMan_Pool::alloc(...) failed due to there is no free chunk exist whit specified size!" < 0);
    return null;
}

SEGAN_INLINE void* MemMan_Pool::realloc(const void* p, const uint sizeInByte)
{
    if (!p)
    {
        return alloc(sizeInByte);
    }
    else
    {
        sx_assert(pbyte(p) > m_pool && pbyte(p) < (m_pool + mem_size(m_pool)));
        void* tmp = alloc(sizeInByte);
        mem_copy(tmp, p, mem_min_size(size(p), sizeInByte));
        free(p);
        return tmp;
    }
}

SEGAN_INLINE void* MemMan_Pool::free(const void* p)
{
    if (!p) return null;
    sx_assert(pbyte(p) > m_pool && pbyte(p) < (m_pool + mem_size(m_pool)));

    Chunk* ch = PChunk(pbyte(p) - sizeof(Chunk));
    sx_assert(ch->state == CS_FULL);	//	avoid to free a chunk more that once

#ifdef _DEBUG
    mem_set((void*)p, 0, ch->size);
#endif

    //  look at neighbor chunk in right side
    Chunk* rch = PChunk(pbyte(p) + ch->size);
    if (rch->state == CS_EMPTY)
    {
        PChunk(pbyte(rch) + sizeof(Chunk) + rch->size)->behind = ch;
        ch->size += sizeof(Chunk) + rch->size;
        pop(rch);

#ifdef _DEBUG
        mem_set(rch, 0, sizeof(Chunk));
#endif
    }

    //  look at neighbor chunk in left side
    if (ch->behind)
    {
        if (ch->behind->state == CS_EMPTY)
        {
            PChunk(pbyte(ch) + sizeof(Chunk) + ch->size)->behind = ch->behind;
            ch->behind->size += sizeof(Chunk) + ch->size;
#ifdef _DEBUG
            mem_set(ch, 0, sizeof(Chunk));
#endif
            return null;	//	this one is already exist in list
        }
    }

    //	push to free list
    ch->state = CS_EMPTY;
    push(ch);
    return null;
}

SEGAN_INLINE uint MemMan_Pool::size(const void* p)
{
    if (!p) return 0;
    sx_assert(pbyte(p) > m_pool && pbyte(p) < (m_pool + mem_size(m_pool)));
    Chunk* ch = PChunk(pbyte(p) - sizeof(Chunk));
    return ch->size;
}


SEGAN_INLINE void MemMan_Pool::push(Chunk* ch)
{
    ch->next = m_root;
    ch->prev = null;
    m_root->prev = ch;
    m_root = ch;
}

SEGAN_INLINE void MemMan_Pool::pop(Chunk* ch)
{
    if (ch->prev)	ch->prev->next = ch->next;
    if (ch->next)	ch->next->prev = ch->prev;
    if (ch == m_root)	m_root = ch->next;
}

#endif
