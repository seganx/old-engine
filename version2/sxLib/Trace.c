#if defined(_WIN32)
#include <Windows.h>
#endif

#include "Trace.h"

#define _GNU_SOURCE
#include <string.h>
#include <time.h>
#include <signal.h>


//////////////////////////////////////////////////////////////////////////
//	simple call stack system
//////////////////////////////////////////////////////////////////////////
#if (SEGANX_TRACE_CALLSTACK || SEGANX_TRACE_PROFILER)
typedef struct trace_info
{
    const char*	func;		    //	function name
    const char*	file;           //  file name
    int	    line;           //  line number
#if SEGANX_TRACE_PROFILER
    long    time;           // time consumed will be computed on return
#endif
#if SEGANX_TRACE_CALLSTACK
    char	param[128];	    //	function parameters
#endif
}
trace_info;
#endif

#if SEGANX_TRACE_PROFILER
typedef struct trace_func_profile 
{

}
trace_func_profile;
#endif

#if (SEGANX_TRACE_CALLSTACK || SEGANX_TRACE_PROFILER || SEGANX_TRACE_CRASHRPT)
typedef struct trace_object
{
#if SEGANX_TRACE_CRASHRPT
    const char*            filename;
#endif
    uint                   callstack_count;
    uint                   callstack_index;
    struct trace_info*     callstack_array;
}
trace_object;

#ifdef _WIN32
__declspec(thread) struct trace_object * s_current_object = null;
#else
static __thread struct trace_object * s_current_object = null;
#endif

#endif


#if SEGANX_TRACE_CRASHRPT
static void trace_set_crash_handler(void);
#endif

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
#endif

#if (SEGANX_TRACE_CALLSTACK || SEGANX_TRACE_PROFILER || SEGANX_TRACE_CRASHRPT)
SEGAN_LIB_API void trace_attach(uint stack_size, const char* filename)
{
    if (s_current_object != null) return;
    s_current_object = (struct trace_object*)calloc(1, sizeof(struct trace_object));
    s_current_object->filename = filename;

#if (SEGANX_TRACE_CALLSTACK || SEGANX_TRACE_PROFILER)
    s_current_object->callstack_count = stack_size;
    s_current_object->callstack_array = (struct trace_info*)calloc(stack_size, sizeof(struct trace_info*));
#endif

    trace_set_crash_handler();
}
#endif

#if (SEGANX_TRACE_CALLSTACK || SEGANX_TRACE_PROFILER)

static SEGAN_INLINE struct trace_info* trace_object_pop()
{
    return &s_current_object->callstack_array[s_current_object->callstack_index++];
}

SEGAN_LIB_API void trace_detach( void )
{
    if (s_current_object)
    {
        if (s_current_object->callstack_array)
            free(s_current_object->callstack_array);
        free(s_current_object);
    }
    s_current_object = null;
}

SEGAN_LIB_API void trace_push( const char * file, const int line, const char * function )
{
    struct trace_info* tinfo = trace_object_pop(s_current_object);
    tinfo->file = (char*)file;
    tinfo->line = line;
    tinfo->func = (char*)function;
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
    tinfo->file = (char*)file;
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
#endif

#if SEGANX_TRACE_PROFILER
    tinfo->time = trace_get_current_tick();
#endif
}

SEGAN_LIB_API void trace_pop( void )
{
#if SEGANX_TRACE_PROFILER
    struct trace_info* tinfo = &s_current_object->callstack_array[--s_current_object->callstack_index];
    tinfo->time = trace_get_current_tick() - tinfo->time;
#else
    --s_current_object->callstack_index
#endif
}

#endif // (SEGANX_TRACE_CALLSTACK || SEGANX_TRACE_PROFILER)

#if SEGANX_TRACE_CRASHRPT

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

    fprintf(fstr, "Received SIG%s (%u)", _strsignal(sig), sig);
    if (sinfo->si_code == SI_USER)
        fprintf(fstr, " sent by user %u from process %u\n", sinfo->si_uid, sinfo->si_pid);
    else if (sinfo->si_code == SI_TKILL)
        fprintf(fstr, " sent by tkill\n");
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

        fprintf(fstr, "Fault at memory location 0x%x due to %s (%x).\n", sinfo->si_addr, reason, sinfo->si_code);
    }

    _exit(EXIT_FAILURE);
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



