#if defined(_WIN32)
#include <Windows.h>
#endif

#include <string.h>
#include <time.h>
#include <signal.h>

#include "Memory.h"
#include "Trace.h"


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#if (SEGANX_TRACE_CALLSTACK || SEGANX_TRACE_PROFILER)
typedef struct trace_info
{
    const char*	func;		    //	function name
    const char*	file;           //  file name
    int	        line;           //  line number
#if SEGANX_TRACE_PROFILER
    long        time;           // time consumed will be computed on return
#endif
#if SEGANX_TRACE_CALLSTACK
    char	    param[128];	    //	function parameters
#endif
}
trace_info;
#endif // (SEGANX_TRACE_CALLSTACK || SEGANX_TRACE_PROFILER)

#if SEGANX_TRACE_PROFILER
typedef struct trace_func_profile 
{

}
trace_func_profile;
#endif // SEGANX_TRACE_PROFILER

#if SEGANX_TRACE_MEMORY

#define MEM_PROTECTION_SIZE     16
#define MEM_PROTECTION_SIGN     "!protected area!"

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

typedef struct memory_tracker
{
    struct memory_block*    root;
    bool                    enable;
    sint                    num_corruptions;
}
memory_tracker;

typedef struct memory_code_result
{
    void*   	            p;		//	user coded memory
    struct memory_block*    mb;		//	memory block for log
}
memory_code_result;

#endif // SEGANX_TRACE_MEMORY

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#if (SEGANX_TRACE_CALLSTACK || SEGANX_TRACE_PROFILER || SEGANX_TRACE_CRASHRPT || SEGANX_TRACE_MEMORY)
typedef struct trace_object
{
#if (SEGANX_TRACE_CRASHRPT || SEGANX_TRACE_MEMORY)
    const char*            filename;
#endif
#if (SEGANX_TRACE_CALLSTACK || SEGANX_TRACE_PROFILER)
    uint                   callstack_count;
    uint                   callstack_index;
    struct trace_info*     callstack_array;
#endif
#if SEGANX_TRACE_MEMORY
    struct memory_tracker *     mem_tracker;
#endif
}
trace_object;

#ifdef _WIN32
__declspec(thread) struct trace_object * s_current_object = null;
#else
static __thread struct trace_object * s_current_object = null;
#endif

static SEGAN_INLINE const char* trace_get_filename(const char* filename)
{
    const char* res = filename;
    for (const char* c = filename; *c != 0; ++c)
        if (*c == '/' || *c == '\\')
            res = c + 1;
    return res;
}

#endif // (SEGANX_TRACE_CALLSTACK || SEGANX_TRACE_PROFILER || SEGANX_TRACE_CRASHRPT || SEGANX_TRACE_MEMORY)


#if SEGANX_TRACE_MEMORY

static SEGAN_INLINE void trace_mem_check(memory_block* mb)
{
    byte* p = (byte*)mb + sizeof(memory_block);

    //	check beginning of memory block
    if (memcmp(p, MEM_PROTECTION_SIGN, MEM_PROTECTION_SIZE))
    {
        mb->corrupted = true;
    }
    else
    {
        //	check end of memory block
        p += MEM_PROTECTION_SIZE + mb->size;
        mb->corrupted = memcmp(p, MEM_PROTECTION_SIGN, MEM_PROTECTION_SIZE) != 0;
    }
}

static SEGAN_INLINE memory_code_result trace_mem_code(void* p, const uint realsizeinbyte)
{
    memory_code_result res;
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
    memcpy(res.mb->sign, MEM_PROTECTION_SIGN, MEM_PROTECTION_SIZE);

    // prepare the result
    p = (byte*)p + sizeof(memory_block);
    res.p = (byte*)p + MEM_PROTECTION_SIZE;

    //	sign beginning of memory block
    memcpy(p, MEM_PROTECTION_SIGN, MEM_PROTECTION_SIZE);

    //	sign end of memory block
    memcpy((byte*)res.p + realsizeinbyte, MEM_PROTECTION_SIGN, MEM_PROTECTION_SIZE);

    return res;
}

static SEGAN_INLINE memory_code_result trace_mem_decode(const void* p)
{
    memory_code_result res;
    if (!p)
    {
        res.p = null;
        res.mb = null;
    }
    else
    {
        res.mb = (memory_block*)((byte*)p - MEM_PROTECTION_SIZE - sizeof(memory_block));
        res.p = (void*)p;

        // check protection sign for memory block
        if (memcmp(res.mb->sign, MEM_PROTECTION_SIGN, MEM_PROTECTION_SIZE))
        {
            res.p = null;
            res.mb = null;
        }
        else trace_mem_check(res.mb);
    }
    return res;
}

static SEGAN_INLINE void trace_mem_push(memory_block* mb)
{
    if (!s_current_object->mem_tracker->root)
    {
        mb->next = null;
        mb->prev = null;
        s_current_object->mem_tracker->root = mb;
    }
    else
    {
        s_current_object->mem_tracker->root->prev = mb;
        mb->next = s_current_object->mem_tracker->root;
        mb->prev = null;
        s_current_object->mem_tracker->root = mb;
    }
}

static SEGAN_INLINE void trace_mem_pop(memory_block* mb)
{
    if (s_current_object->mem_tracker->root)
    {
        // unlink from the list
        if (mb == s_current_object->mem_tracker->root)
        {
            s_current_object->mem_tracker->root = s_current_object->mem_tracker->root->next;
            if (s_current_object->mem_tracker->root)
                s_current_object->mem_tracker->root->prev = null;
        }
        else
        {
            mb->prev->next = mb->next;
            if (mb->next)
                mb->next->prev = mb->prev;
        }
    }
}

static uint trace_mem_report_compute_num(bool only_corrupted)
{
    if (!s_current_object->mem_tracker->root) return 0;

    uint result = 0;
    memory_block* leaf = s_current_object->mem_tracker->root;
    while (leaf)
    {
        trace_mem_check(leaf);
        if (only_corrupted)
        {
            if (leaf->corrupted)
                result++;
        }
        else result++;
        leaf = leaf->next;
    }

    return result;
}

static void trace_mem_report(FILE* f, bool only_corrupted)
{
    if (!s_current_object->mem_tracker->root || trace_mem_report_compute_num(only_corrupted) < 1) return;
    fputs("\nseganx memory debug report:\n", f);
    struct memory_block* leaf = s_current_object->mem_tracker->root;
    while (leaf)
    {
        trace_mem_check(leaf);
        if (only_corrupted && !leaf->corrupted)
        {
            leaf = leaf->next;
            continue;
        }
#ifndef NDEBUG
        if (leaf->corrupted)
            fprintf(f, "%s(%d): error! memory corrupted - size = %d\n", leaf->file, leaf->line, leaf->size);
        else
            fprintf(f, "%s(%d): warning! memory leak - size = %d\n", leaf->file, leaf->line, leaf->size);
#else
        if (leaf->corrupted)
            fprintf(f, "%s(%d): error! memory corrupted - size = %d\n", trace_get_filename(leaf->file), leaf->line, leaf->size);
        else
            fprintf(f, "%s(%d): warning! memory leak - size = %d\n", trace_get_filename(leaf->file), leaf->line, leaf->size);
#endif
        leaf = leaf->next;
    }
}

SEGAN_INLINE void* trace_mem_alloc(const uint size_in_byte, const char* file, const int line)
{
    void* res = null;

    if (s_current_object->mem_tracker->enable)
    {
        //	first block for holding data, second block for protection and memory, then close with other protection
        res = mem_alloc(sizeof(memory_block) + MEM_PROTECTION_SIZE + size_in_byte + MEM_PROTECTION_SIZE);
        sx_assert(res);

        //	sign memory to check memory corruption
        memory_code_result memreport = trace_mem_code(res, size_in_byte);

        //	store memory block to link list
        if (memreport.mb)
        {
            memreport.mb->file = (char*)file;
            memreport.mb->line = line;
            memreport.mb->size = size_in_byte;

            // push the block
            trace_mem_push(memreport.mb);
        }

        //	set user memory as result
        res = memreport.p;
    }
    else res = mem_alloc(size_in_byte);

    return res;
}

SEGAN_INLINE void* trace_mem_realloc(void* p, const uint new_size_in_byte, const char* file, const int line)
{
    if (!new_size_in_byte)
        return trace_mem_free(p);

    memory_code_result memreport = trace_mem_decode(p);

    if (memreport.mb)
    {
        //	if memory has been corrupted we should hold the corrupted memory info
        if (memreport.mb->corrupted)
        {
            //	report memory allocations to file
            trace_mem_report(stderr, true);

#if ( defined(_DEBUG) || SEGAN_ASSERT )
            //	report call stack
            lib_assert("memory block has been corrupted !", memreport.mb->file, memreport.mb->line);
#endif
        }
        else
        {
            //	pop memory block from the list
            trace_mem_pop(memreport.mb);
        }

        //	realloc the memory block
        void* tmp = mem_realloc(memreport.mb, sizeof(memory_block) + MEM_PROTECTION_SIZE + new_size_in_byte + MEM_PROTECTION_SIZE);

        //	sign memory to check protection
        memreport = trace_mem_code(tmp, new_size_in_byte);

        if (memreport.mb)
        {
            memreport.mb->file = (char*)file;
            memreport.mb->line = line;
            memreport.mb->size = new_size_in_byte;

            //	push new block
            trace_mem_push(memreport.mb);
        }

        //	set as result
        return memreport.p;
    }
    else
    {
        if (s_current_object->mem_tracker->enable)
        {
            //	realloc the memory block
            p = mem_realloc(p, sizeof(memory_block) + MEM_PROTECTION_SIZE + new_size_in_byte + MEM_PROTECTION_SIZE);

            //	sign memory to check protection
            memreport = trace_mem_code(p, new_size_in_byte);

            if (memreport.mb)
            {
                memreport.mb->file = (char*)file;
                memreport.mb->line = line;
                memreport.mb->size = new_size_in_byte;

                //	push the block
                trace_mem_push(memreport.mb);
            }

            //	set as result
            return memreport.p;
        }
    }

    return mem_realloc(p, new_size_in_byte);
}

SEGAN_INLINE void* trace_mem_free(const void* p)
{
    if (!p) return null;

    memory_code_result memreport = trace_mem_decode(p);

    if (memreport.mb)
    {
        //	if memory has been corrupted we should hold the corrupted memory info
        if (memreport.mb->corrupted)
        {
            //	report memory allocations to file
            trace_mem_report(stderr, true);

#if ( defined(_DEBUG) || SEGAN_ASSERT )
            //	report call stack
            lib_assert("memory block has been corrupted !", memreport.mb->file, memreport.mb->line);
#endif
        }
        else
        {
            //	pop memory block from the list
            trace_mem_pop(memreport.mb);
            mem_free(memreport.mb);
        }
    }
    else mem_free(p);

    return null;
}

#endif // SEGANX_TRACE_MEMORY


#if SEGANX_TRACE_CRASHRPT
static void trace_set_crash_handler(void);
#endif // SEGANX_TRACE_CRASHRPT


#if SEGANX_TRACE_PROFILER
static SEGAN_INLINE long trace_get_current_tick() 
{
#ifdef _WIN32
#else
    struct timespec res;
    if (clock_gettime(CLOCK_REALTIME, &res) == 0)
        return res.tv_nsec;
    else
        return 0;
#endif
}
#endif // SEGANX_TRACE_PROFILER


#if (SEGANX_TRACE_CALLSTACK || SEGANX_TRACE_PROFILER || SEGANX_TRACE_CRASHRPT || SEGANX_TRACE_MEMORY)
SEGAN_LIB_API void trace_attach(uint stack_size, const char* filename)
{
    if (s_current_object != null) return;
    s_current_object = (struct trace_object*)calloc(1, sizeof(struct trace_object));
    s_current_object->filename = filename;

#if (SEGANX_TRACE_CALLSTACK || SEGANX_TRACE_PROFILER)
    s_current_object->callstack_count = stack_size;
    s_current_object->callstack_array = (struct trace_info*)calloc(stack_size, sizeof(struct trace_info));
#endif

#if SEGANX_TRACE_MEMORY
    s_current_object->mem_tracker = (struct memory_tracker*)calloc(1, sizeof(struct memory_tracker));
    s_current_object->mem_tracker->enable = true;
#endif

#if SEGANX_TRACE_CRASHRPT
    trace_set_crash_handler();
#endif
}

SEGAN_LIB_API void trace_detach(void)
{
    if (s_current_object)
    {
#if SEGANX_TRACE_MEMORY
        trace_mem_report(stderr, false);
        if (s_current_object->filename && trace_mem_report_compute_num(false) > 0)
        {
            FILE* fstr = fopen(s_current_object->filename, "a+, ccs=UTF-8");
            if (fstr) {
                trace_mem_report(fstr, false);
                fclose(fstr);
            }
        }
        free(s_current_object->mem_tracker);
#endif // SEGANX_TRACE_MEMORY

#if (SEGANX_TRACE_CALLSTACK || SEGANX_TRACE_PROFILER)
        if (s_current_object->callstack_array)
            free(s_current_object->callstack_array);
#endif  //  (SEGANX_TRACE_CALLSTACK || SEGANX_TRACE_PROFILER)
        free(s_current_object);
    }
    s_current_object = null;
}
#endif // SEGANX_TRACE_CALLSTACK || SEGANX_TRACE_PROFILER || SEGANX_TRACE_CRASHRPT|| SEGANX_TRACE_MEMORY


#if (SEGANX_TRACE_CALLSTACK || SEGANX_TRACE_PROFILER)
static SEGAN_INLINE struct trace_info* trace_object_pop()
{
    return &s_current_object->callstack_array[s_current_object->callstack_index++];
}

SEGAN_LIB_API void trace_push( const char * file, const int line, const char * function )
{
    struct trace_info* tinfo = trace_object_pop(s_current_object);
    tinfo->file = file;
    tinfo->line = line;
    tinfo->func = function;
#if SEGANX_TRACE_CALLSTACK
    tinfo->param[0] = 0;
#endif
#if SEGANX_TRACE_PROFILER
    tinfo->time = trace_get_current_tick();
#endif
}

SEGAN_LIB_API void trace_push_param( const char * file, const int line, const char * function, ... )
{
    struct trace_info* tinfo = trace_object_pop(s_current_object);
    tinfo->file = file;
    tinfo->line = line;

#if SEGANX_TRACE_CALLSTACK
    tinfo->func = null;
    if (function)
    {
        va_list args;
        va_start(args, function);
        sint len = vsnprintf(0, 0, function, args) + 1;
        if (len < 128)
            vsnprintf(tinfo->param, 128, function, args);
        else 
            tinfo->func = function;
        va_end(args);
    }
    else tinfo->param[0] = 0;
#else
    tinfo->func = function;
#endif // SEGANX_TRACE_CALLSTACK

#if SEGANX_TRACE_PROFILER
    tinfo->time = trace_get_current_tick();
#endif // SEGANX_TRACE_PROFILER
}

SEGAN_LIB_API void trace_pop( void )
{
#if SEGANX_TRACE_PROFILER
    struct trace_info* tinfo = &s_current_object->callstack_array[--s_current_object->callstack_index];
    tinfo->time = trace_get_current_tick() - tinfo->time;
#else
    --s_current_object->callstack_index;
#endif
}

#endif // (SEGANX_TRACE_CALLSTACK || SEGANX_TRACE_PROFILER)


#if SEGANX_TRACE_CRASHRPT

#if SEGANX_TRACE_CALLSTACK
static void trace_callstack_report(FILE* f) 
{
    fprintf(f, "\nUser defined callstack:\n");
    for (int i = 0; i < s_current_object->callstack_index; ++i)
    {
        for (int j = 0; j < i; ++j) fprintf(f, "  ");
#ifndef NDEBUG
        fprintf(f, "%s(%d): %s\n", s_current_object->callstack_array[i].file, s_current_object->callstack_array[i].line, s_current_object->callstack_array[i].func);
#else
        fprintf(f, "%s(%d): %s\n", trace_get_filename(s_current_object->callstack_array[i].file), s_current_object->callstack_array[i].line, s_current_object->callstack_array[i].func);
#endif
    }
}
#endif // SEGANX_TRACE_CALLSTACK

#ifdef _WIN32
#define  CRASHED_UNKNOWN                    "Application crashes unexpectedly!"
#define  CRASHED_MEMORY_ALLOC               "Can't allocate sufficient memory!"
#define  CRASHED_APPLICATION_TERMINATED     "Application was terminated!"
#define  CRASHED_ACCESS_VIOLATION           "Access Violation! The thread tried to read from or write to a virtual address for which it does not have the appropriate access."
#define  CRASHED_EXTERNAL_INTERRUPT         "Application has requested the Runtime to terminate it in an unusual way. Usually initiated by the user Ctrl+C."
#define  CRASHED_INVALID_IMAGE              "Invalid program image, such as invalid instruction!"
#define  CRASHED_ABNORMAL_TERMINATION       "Abnormal termination condition, as is e.g. initiated by std::abort()!"
#define  CRASHED_ARITHMETIC_OPERATION       "Erroneous arithmetic operation such as divide by zero!"
#define  CRASHED_PURE_CALL                  "The thread tried to call an abstracted uninitialized virtual function!"
#define  CRASHED_INVALID_PARAMETER          "Call a function with invalid parameter!"
#define  CRASHED_ARRAY_BOUNDS               "The thread tried to access an array element that is out of bounds and the underlying hardware supports bounds checking."
#define  CRASHED_DATATYPE_MISALIGNMENT      "The thread tried to read or write data that is misaligned on hardware that does not provide alignment. For example, 16-bit values must be aligned on 2-byte boundaries; 32-bit values on 4-byte boundaries, and so on."
#define  CRASHED_FLT_DENORMAL               "One of the operands in a floating-point operation is denormal. A denormal value is one that is too small to represent as a standard floating-point value."
#define  CRASHED_FLT_DIVIDE_BY_ZERO         "The thread tried to divide a floating-point value by a floating-point divisor of zero."
#define  CRASHED_FLT_INEXACT                "The result of a floating-point operation cannot be represented exactly as a decimal fraction."
#define  CRASHED_FLT_INVALID                "This exception represents any floating-point exception not recognized!"
#define  CRASHED_FLT_OVERFLOW               "The exponent of a floating-point operation is greater than the magnitude allowed by the corresponding type."
#define  CRASHED_FLT_STACK_CHECK            "The stack overflowed or underflowed as the result of a floating-point operation."
#define  CRASHED_FLT_UNDERFLOW              "The exponent of a floating-point operation is less than the magnitude allowed by the corresponding type."
#define  CRASHED_ILLEGAL_INSTRUCTION        "The thread tried to execute an invalid instruction."
#define  CRASHED_INT_DIVIDE_BY_ZERO         "The thread tried to divide an integer value by an integer divisor of zero."
#define  CRASHED_INT_OVERFLOW               "The result of an integer operation caused a carry out of the most significant bit of the result."
#define  CRASHED_NONCONTINUABLE_EXCEPTION   "The thread tried to continue execution after a noncontinuable exception occurred."
#define  CRASHED_PRIV_INSTRUCTION           "The thread tried to execute an instruction whose operation is not allowed in the current machine mode."
#define  CRASHED_SINGLE_STEP                "A trace trap or other single-instruction mechanism signaled that one instruction has been executed."
#define  CRASHED_STACK_OVERFLOW             "The thread/process used up its stack. Stack overflow!"

static void trace_app_crashed(const char* reason, dword code)
{
    printf("code: %d - %s", code, reason);
}

static void trace_crash_signal_handler(int sig)
{
    switch (sig)
    {
    case SIGTERM:	trace_app_crashed(CRASHED_APPLICATION_TERMINATED, 0); break;
    case SIGSEGV:	trace_app_crashed(CRASHED_ACCESS_VIOLATION, 0); break;
    case SIGINT:	trace_app_crashed(CRASHED_EXTERNAL_INTERRUPT, 0); break;
    case SIGILL:	trace_app_crashed(CRASHED_INVALID_IMAGE, 0); break;
    case SIGABRT:	trace_app_crashed(CRASHED_ABNORMAL_TERMINATION, 0); break;
    case SIGFPE:	trace_app_crashed(CRASHED_ARITHMETIC_OPERATION, 0); break;
    }
}

LONG WINAPI crash_seh_handler(__in PEXCEPTION_POINTERS pExceptionPtrs)
{
    dword ecode = pExceptionPtrs->ExceptionRecord->ExceptionCode;
    switch (ecode)
    {
    case EXCEPTION_ACCESS_VIOLATION:		crash_call_callback(ECR_ACCESS_VIOLATION, ecode); break;
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:	crash_call_callback(ECR_ARRAY_BOUNDS, ecode); break;
    case EXCEPTION_DATATYPE_MISALIGNMENT:	crash_call_callback(ECR_DATATYPE_MISALIGNMENT, ecode); break;
    case EXCEPTION_FLT_DENORMAL_OPERAND:	crash_call_callback(ECR_FLT_DENORMAL, ecode); break;
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:		crash_call_callback(ECR_FLT_DIVIDE_BY_ZERO, ecode); break;
    case EXCEPTION_FLT_INEXACT_RESULT:		crash_call_callback(ECR_FLT_INEXACT, ecode); break;
    case EXCEPTION_FLT_INVALID_OPERATION:	crash_call_callback(ECR_FLT_INVALID, ecode); break;
    case EXCEPTION_FLT_OVERFLOW:			crash_call_callback(ECR_FLT_OVERFLOW, ecode); break;
    case EXCEPTION_FLT_STACK_CHECK:			crash_call_callback(ECR_FLT_STACK_CHECK, ecode); break;
    case EXCEPTION_FLT_UNDERFLOW:			crash_call_callback(ECR_FLT_UNDERFLOW, ecode); break;
    case EXCEPTION_ILLEGAL_INSTRUCTION:		crash_call_callback(ECR_ILLEGAL_INSTRUCTION, ecode); break;
    case EXCEPTION_INT_DIVIDE_BY_ZERO:		crash_call_callback(ECR_INT_DIVIDE_BY_ZERO, ecode); break;
    case EXCEPTION_INT_OVERFLOW:			crash_call_callback(ECR_INT_OVERFLOW, ecode); break;
    case EXCEPTION_PRIV_INSTRUCTION:		crash_call_callback(ECR_PRIV_INSTRUCTION, ecode); break;
    case EXCEPTION_SINGLE_STEP:				crash_call_callback(ECR_SINGLE_STEP, ecode); break;
    case EXCEPTION_STACK_OVERFLOW:			crash_call_callback(ECR_STACK_OVERFLOW, ecode); break;

    default: crash_call_callback(ECR_UNKNOWN, ecode);
    }
    return 0;
}
#endif // _WIN32

#ifdef __linux__
static void trace_signal_handler(int sig, siginfo_t* sinfo, void *ucontext)
{
    FILE* fstr = fopen(s_current_object->filename, "a+, ccs=UTF-8");
    if (fstr == null)
        fstr = stderr;

    // TODO: Add time alog the string below..
    fprintf(fstr, "\n\nseganx crash report:\nProgram received %s (%u)", strsignal(sig), sig);
    if (sinfo->si_code == SI_USER)
        fprintf(fstr, " sent by user %u from process %u", sinfo->si_uid, sinfo->si_pid);
    else if (sinfo->si_code == SI_TKILL)
        fprintf(fstr, " sent by tkill");
    else if (sinfo->si_code == SI_KERNEL)
        fprintf(fstr, " sent by kernel");
    else 
    {
        // signal translation from: http://uw714doc.sco.com/en/man/html.5/siginfo.5.html
        const char* reason = null;
        switch (sig) 
        {
        case SIGILL: 
        {
            switch (sinfo->si_code) 
            {
            case ILL_ILLOPC: reason = "illegal opcode"; break;
            case ILL_ILLOPN: reason = "illegal operand"; break;
            case ILL_ILLADR: reason = "illegal addressing mode"; break;
            case ILL_ILLTRP: reason = "illegal trap"; break;
            case ILL_PRVOPC: reason = "privileged opcode"; break;
            case ILL_PRVREG: reason = "privileged register"; break;
            case ILL_COPROC: reason = "co-processor error"; break;
            case ILL_BADSTK: reason = "internal stack error"; break;
            default: reason = "unknown"; break;
            }
        }
        break;
        case SIGFPE: 
        {
            switch (sinfo->si_code) 
            {
            case FPE_INTDIV: reason = "integer divide by zero"; break;
            case FPE_INTOVF: reason = "integer overflow"; break;
            case FPE_FLTDIV: reason = "floating point divide by zero"; break;
            case FPE_FLTOVF: reason = "floating point overflow"; break;
            case FPE_FLTUND: reason = "floating point underflow"; break;
            case FPE_FLTRES: reason = "floating point inexact result"; break;
            case FPE_FLTINV: reason = "floating point invalid operation"; break;
            case FPE_FLTSUB: reason = "subscript out of range"; break;
            default: reason = "unknown"; break;
            }
        }
        break;
        case SIGSEGV: 
        {
            switch (sinfo->si_code) 
            {
            case SEGV_MAPERR: reason = "address not mapped to object"; break;
            case SEGV_ACCERR: reason = "invalid permissions for mapped object"; break;
            default: reason = "unknown"; break;
            }
        }
        break;
        case SIGBUS: 
        {
            switch (sinfo->si_code) 
            {
            case BUS_ADRALN: reason = "invalid address alignment"; break;
            case BUS_ADRERR: reason = "non-existent physical address"; break;
            case BUS_OBJERR: reason = "object-specific hardware error"; break;
            default: reason = "unknown"; break;
            }
        }
        break;
        case SIGPOLL:
        {
            switch (sinfo->si_code)
            {
            case POLL_IN: reason = "data input available"; break;
            case POLL_OUT: reason = "output buffers available"; break;
            case POLL_MSG: reason = "input message available"; break;
            case POLL_ERR: reason = "I/O error"; break;
            case POLL_PRI: reason = "high priority input available"; break;
            case POLL_HUP: reason = "device disconnected"; break;
            default: reason = "unknown"; break;
            }
        }
        break;

        case SIGABRT: reason = "abnormal termination condition"; break;
        case SIGINT: reason = "application has requested the Runtime to terminate it in an unusual way. Usually initiated by the user Ctrl + C"; break;        
        case SIGTERM: reason = "application was terminated"; break;
        default: reason = "unknown"; break;
        }

        fprintf(fstr, "\nFault at memory location 0x%x due to %s (%x).\n", sinfo->si_addr, reason, sinfo->si_code);
    }

#if SEGANX_TRACE_CALLSTACK
    trace_callstack_report(fstr);
#endif // SEGANX_TRACE_CALLSTACK

#if SEGANX_TRACE_MEMORY
    trace_mem_report(fstr, true);
#endif // SEGANX_TRACE_MEMORY

    exit(EXIT_FAILURE);
}

#endif // __linux__

static void trace_set_crash_handler(void)
{
#ifdef _WIN32
    SetUnhandledExceptionFilter(crash_seh_handler);
    signal(SIGTERM, trace_crash_signal_handler);
    signal(SIGSEGV, trace_crash_signal_handler);
    signal(SIGINT, trace_crash_signal_handler);
    signal(SIGILL, trace_crash_signal_handler);
    signal(SIGABRT, trace_crash_signal_handler);
    signal(SIGFPE, trace_crash_signal_handler);
#endif
#ifdef __linux__
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGABRT);
    sigaddset(&sigset, SIGBUS);
    sigaddset(&sigset, SIGILL);
    sigaddset(&sigset, SIGSEGV);
    sigaddset(&sigset, SIGFPE);

    struct sigaction sa;
    sa.sa_sigaction = trace_signal_handler;
    sa.sa_mask = sigset;
    sa.sa_flags = SA_SIGINFO | SA_ONSTACK;

    sigaction(SIGABRT, &sa, 0);
    sigaction(SIGBUS, &sa, 0);
    sigaction(SIGILL, &sa, 0);
    sigaction(SIGSEGV, &sa, 0);
    sigaction(SIGFPE, &sa, 0);
#endif
}

#endif


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#if SEGANX_TRACE_ASSERT

SEGAN_LIB_API void trace_assert(const char* expression, const char* file, const int line)
{
    trace_push(file, line, expression);
    FILE* fstr = fopen(s_current_object->filename, "a+, ccs=UTF-8");
    if (fstr == null) fstr = stderr;
    fprintf(fstr, "\n\nseganx assertion report:\n%s(%d): Assertion violation on expression (%s)\n", trace_get_filename(file), line, expression);

#if SEGANX_TRACE_CALLSTACK
    trace_callstack_report(fstr);
#endif // SEGANX_TRACE_CALLSTACK

#if SEGANX_TRACE_MEMORY
    trace_mem_report(fstr, true);
#endif // SEGANX_TRACE_MEMORY
    
#ifndef NDEBUG 
    int* a = 0;
    *a = 0; //	just move your eyes down and look at the call stack list in IDE to find out what happened !
#endif
}

#endif // SEGANX_TRACE_ASSERT

