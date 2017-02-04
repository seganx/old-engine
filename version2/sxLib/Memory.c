#if defined(_WIN32)
#include <Windows.h>
#endif
#include <string.h>

#include "Memory.h"
#include "Assert.h"


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
//	memory management
//////////////////////////////////////////////////////////////////////////
static struct sx_memory_manager* s_manager = null;

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
//	debug allocator used to detect memory leaks
#if ( SEGAN_MEMLEAK == 1 )

typedef struct memory_block
{
    char                    sign[16];		//! sign to check if the block is a memory block. It's different from protection sign
    char*                   file;
    int                     line;
    uint                    size;
    bool                    corrupted;
    struct memory_block*    next;
    struct memory_block*    prev;
}
memory_block;

static struct memory_block*     s_mem_root = null;
static bool                     s_mem_enable_leak = true;
static sint                     s_mem_corruptions = 0;
static const uint               s_mem_protect_size = 16;
static const char*              s_mem_protection = "!protected area!";


typedef struct MemCodeReport
{
    void*		            p;		//	user coded memory
    struct memory_block*	mb;		//	memory block for log
}
MemCodeReport;

static SEGAN_INLINE void mem_block_to_str(char* dest, const uint dest_size, const memory_block* mb)
{
    if (mb->corrupted)
        snprintf(dest, dest_size, "%s(%d): error! memory corrupted - size = %d\n", mb->file, mb->line, mb->size);
    else
        snprintf(dest, dest_size, "%s(%d): warning! memory leak - size = %d\n", mb->file, mb->line, mb->size);
}

static SEGAN_INLINE void mem_check_protection(memory_block* mb)
{
    byte* p = (byte*)mb + sizeof(memory_block);

    //	check beginning of memory block
    if (memcmp(p, s_mem_protection, s_mem_protect_size))
    {
        mb->corrupted = true;
    }
    else
    {
        //	check end of memory block
        p += s_mem_protect_size + mb->size;
        mb->corrupted = memcmp(p, s_mem_protection, s_mem_protect_size) != 0;
    }
}

static SEGAN_INLINE MemCodeReport mem_code_protection(void* p, const uint realsizeinbyte)
{
    MemCodeReport res;
    if (!p)
    {
        res.p = null;
        res.mb = null;
        return res;
    }

    // extract memory block
    res.mb = (memory_block*)p;
    res.mb->corrupted = false;

    //	set protection sign for memory block
    memcpy(res.mb->sign, s_mem_protection, s_mem_protect_size);

    // prepare the result
    p = (byte*)p + sizeof(memory_block);
    res.p = (byte*)p + s_mem_protect_size;

    //	sign beginning of memory block
    memcpy(p, s_mem_protection, s_mem_protect_size);

    //	sign end of memory block
    memcpy((byte*)res.p + realsizeinbyte, s_mem_protection, s_mem_protect_size);

    return res;
}


static SEGAN_INLINE MemCodeReport mem_decode_protection(const void* p)
{
    MemCodeReport res;
    if (!p)
    {
        res.p = null;
        res.mb = null;
    }
    else
    {
        res.mb = (memory_block*)((byte*)p - s_mem_protect_size - sizeof(memory_block));
        res.p = (void*)p;

        // check protection sign for memory block
        if (memcmp(res.mb->sign, s_mem_protection, s_mem_protect_size))
        {
            res.p = null;
            res.mb = null;
        }
        else mem_check_protection(res.mb);
    }
    return res;
}

static SEGAN_INLINE void mem_debug_push_unsafe(memory_block* mb)
{
    if (!s_mem_root)
    {
        mb->next = null;
        mb->prev = null;
        s_mem_root = mb;
    }
    else
    {
        s_mem_root->prev = mb;
        mb->next = s_mem_root;
        mb->prev = null;
        s_mem_root = mb;
    }
}

static SEGAN_INLINE void mem_debug_pop_unsafe(memory_block* mb)
{
    if (s_mem_root)
    {
        // unlink from the list
        if (mb == s_mem_root)
        {
            s_mem_root = s_mem_root->next;
            if (s_mem_root)
                s_mem_root->prev = null;
        }
        else
        {
            mb->prev->next = mb->next;
            if (mb->next)
                mb->next->prev = mb->prev;
        }
    }
}

static SEGAN_INLINE void mem_debug_push_pop(memory_block* mb, const bool pop /*= false*/)
{
#if SEGAN_LIB_MULTI_THREADED
    static Mutex s_mutex;
    s_mutex.lock();
#endif

    if (pop)
        mem_debug_pop_unsafe(mb);
    else
        mem_debug_push_unsafe(mb);

#if SEGAN_LIB_MULTI_THREADED
    s_mutex.unlock();
#endif
}


SEGAN_INLINE void mem_enable_debug(const bool enable)
{
    s_mem_enable_leak = enable;
}

SEGAN_INLINE void* mem_alloc_dbg(const uint size_in_byte, const char* file, const int line)
{
    void* res = null;

    if (s_mem_enable_leak)
    {
        //	first block for holding data, second block for protection and memory, then close with other protection
        res = mem_alloc(sizeof(memory_block) + s_mem_protect_size + size_in_byte + s_mem_protect_size);
        sx_assert(res);

        //	sign memory to check memory corruption
        MemCodeReport memreport = mem_code_protection(res, size_in_byte);

        //	store memory block to link list
        if (memreport.mb)
        {
            memreport.mb->file = (char*)file;
            memreport.mb->line = line;
            memreport.mb->size = size_in_byte;

            // push the block
            mem_debug_push_pop(memreport.mb, false);
        }

        //	set user memory as result
        res = memreport.p;
    }
    else res = mem_alloc(size_in_byte);

    return res;
}

SEGAN_INLINE void* mem_realloc_dbg(void* p, const uint new_size_in_byte, const char* file, const int line)
{
    if (!new_size_in_byte)
        return mem_free_dbg(p);

    MemCodeReport memreport = mem_decode_protection(p);

    if (memreport.mb)
    {
        //	if memory has been corrupted we should hold the corrupted memory info
        if (memreport.mb->corrupted)
        {
            //	report memory allocations to file
            char tmp[64] = { 0 };
            snprintf(tmp, 64, "sx_mem_report_%d.txt", s_mem_corruptions);
            mem_report_debug_to_file(tmp);

#if ( defined(_DEBUG) || SEGAN_ASSERT )
            //	report call stack
            lib_assert("memory block has been corrupted !", memreport.mb->file, memreport.mb->line);
#endif
        }
        else
        {
            //	pop memory block from the list
            mem_debug_push_pop(memreport.mb, true);
        }

        //	realloc the memory block
        void* tmp = mem_realloc(memreport.mb, sizeof(memory_block) + s_mem_protect_size + new_size_in_byte + s_mem_protect_size);

        //	sign memory to check protection
        memreport = mem_code_protection(tmp, new_size_in_byte);

        if (memreport.mb)
        {
            memreport.mb->file = (char*)file;
            memreport.mb->line = line;
            memreport.mb->size = new_size_in_byte;

            //	push new block
            mem_debug_push_pop(memreport.mb, false);
        }

        //	set as result
        return memreport.p;
    }
    else
    {
        if (s_mem_enable_leak)
        {
            //	realloc the memory block
            p = mem_realloc(p, sizeof(memory_block) + s_mem_protect_size + new_size_in_byte + s_mem_protect_size);

            //	sign memory to check protection
            memreport = mem_code_protection(p, new_size_in_byte);

            if (memreport.mb)
            {
                memreport.mb->file = (char*)file;
                memreport.mb->line = line;
                memreport.mb->size = new_size_in_byte;

                //	push the block
                mem_debug_push_pop(memreport.mb, false);
            }

            //	set as result
            return memreport.p;
        }
    }

    return mem_realloc(p, new_size_in_byte);
}

SEGAN_INLINE void* mem_free_dbg(const void* p)
{
    if (!p) return null;

    MemCodeReport memreport = mem_decode_protection(p);

    if (memreport.mb)
    {
        //	if memory has been corrupted we should hold the corrupted memory info
        if (memreport.mb->corrupted)
        {
            //	report memory allocations to file
            char tmp[64] = { 0 };
            snprintf(tmp, 64, "sx_mem_report_%d.txt", s_mem_corruptions);
            mem_report_debug_to_file(tmp);

#if ( defined(_DEBUG) || SEGAN_ASSERT )
            //	report call stack
            lib_assert("memory block has been corrupted !", memreport.mb->file, memreport.mb->line);
#endif
        }
        else
        {
            //	pop memory block from the list
            mem_debug_push_pop(memreport.mb, true);
            mem_free(memreport.mb);
        }
    }
    else mem_free(p);

    return null;
}

uint mem_report_compute_num(void)
{
    if (!s_mem_root) return 0;

    uint result = 0;
    memory_block* leaf = s_mem_root;
    while (leaf)
    {
        mem_check_protection(leaf);
        result++;
        leaf = leaf->next;
    }

    return result;
}

SEGAN_INLINE void mem_report_debug(mem_callback callback, void* userdata)
{
    if (!s_mem_root || !callback) return;

    memory_block* leaf = s_mem_root;
    while (leaf)
    {
        mem_check_protection(leaf);
        (*callback)(userdata, leaf->file, leaf->line, leaf->size, leaf->corrupted);
        leaf = leaf->next;
    }
}

#if defined(DEBUG_OUTPUT_WINDOW)
void mem_report_debug_to_window()
{
    if (!s_mem_root) return;

    OutputDebugString(L"seganx memory report : \n");

    memory_block* leaf = s_mem_root;
    while (leaf)
    {
        mem_check_protection(leaf);
        wchar tmp[1024] = { 0 };
        mem_block_to_str(tmp, 1024, leaf);
        OutputDebugString(tmp);
        leaf = leaf->next;
    }
}
#endif

SEGAN_INLINE void mem_report_debug_to_file(const char* fileName)
{
    if (!s_mem_root) return;

#if defined(DEBUG_OUTPUT_WINDOW)
    mem_report_debug_to_window(0);
#endif

    if (mem_report_compute_num() < 1) return;

    //	report called functions in the file
    FILE* f = fopen(fileName, "w, ccs=UTF8");
    if (f)
    {
        fputs("seganx memory debug report : \n\n", f);
        memory_block* leaf = s_mem_root;
        while (leaf)
        {
            mem_check_protection(leaf);
            char tmp[1024] = { 0 };
            mem_block_to_str(tmp, 1024, leaf);
            fputs(tmp, f);
            leaf = leaf->next;
        }
        
        fclose(f);
    }
}

SEGAN_INLINE void mem_clear_debug(void)
{
    memory_block* leaf = s_mem_root;
    while (leaf)
    {
        s_mem_root = leaf->next;
        mem_free(leaf);
        leaf = s_mem_root;
    }
}

#endif

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////




#if 0
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
