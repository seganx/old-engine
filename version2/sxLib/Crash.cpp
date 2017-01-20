#if defined(_WIN32)
#include <Windows.h>
#include <new.h>
#else
#include <new>
#endif
#include <signal.h>
#include <exception>

#include "Crash.h"

#if SEGAN_LIB_MULTI_THREADED
#include "Thread.h"
#endif

#if SEGAN_CRASHRPT

#if SEGAN_CRASHRPT_CALLSTACK

//////////////////////////////////////////////////////////////////////////
//	simple call stack system
//////////////////////////////////////////////////////////////////////////
/* call stack system should be safe. o memory allocation or using
any other service so we need a pool and fill it consecutively. */
int callstack_clear(void);
#define CALLSTACK_MAX	128

CrashCallStack	s_callstack_pool[CALLSTACK_MAX];
uint			s_callstack_index = callstack_clear();	//	a trick to initialize call-stack pool

SEGAN_INLINE void callstack_clean(CrashCallStack *csd)
{
	csd->param[0] = 0;
	csd->func = null;
	csd->file = null;
	csd->line = 0;
}

int callstack_clear(void)
{
	for (int i = 0; i < CALLSTACK_MAX; i++)
		callstack_clean(&s_callstack_pool[i]);
	return 0;
}

CrashCallStack* callstack_push_pop(bool push)
{
	CrashCallStack* res = null;

#if SEGAN_LIB_MULTI_THREADED
	static Mutex s_mutex;
	s_mutex.lock();
#endif

	if (push)
	{
		if (s_callstack_index > 0)
			callstack_clean(&s_callstack_pool[--s_callstack_index]);
	}
	else
	{
		if (s_callstack_index < CALLSTACK_MAX)
			res = &s_callstack_pool[s_callstack_index++];
	}

#if SEGAN_LIB_MULTI_THREADED
	s_mutex.unlock();
#endif

	return res;
}

SEGAN_INLINE _CallStack::_CallStack(const char* file, const sint line, const char* callfunction)
{
	CrashCallStack* csd = callstack_push_pop(false);
	if (csd)
	{
		csd->param[0] = 0;
		csd->file = (char*)file;
		csd->line = line;
		csd->func = (char*)callfunction;
	}
}

SEGAN_INLINE _CallStack::_CallStack(const sint line, const char* file, const char* callfunction, ...)
{
	CrashCallStack* csd = callstack_push_pop(false);
	if (csd)
	{
		csd->file = (char*)file;
		csd->line = line;
		csd->func = null;
		if (callfunction)
		{
			va_list argList;
			va_start(argList, callfunction);
			sint len = sx_vscprintf_len(callfunction, argList);
			if (len < 255)
			{
				sx_vscprintf(csd->param, 255, callfunction, argList);
			}
			else
			{
				uint i = 0;
				for (; i < 255 && callfunction[i]; ++i)
					csd->param[i] = callfunction[i];
				csd->param[i] = 0;
			}
			va_end(argList);
		}
		else csd->param[0] = 0;
	}
}

SEGAN_INLINE _CallStack::~_CallStack(void)
{
	callstack_push_pop(true);
}

#endif // SEGAN_CRASHRPT_CALLSTACK

//////////////////////////////////////////////////////////////////////////
//	simple crash handler system
//////////////////////////////////////////////////////////////////////////

static CrashCallback s_crash_callback = null;

void crash_call_callback(ECrashReason reason, dword code = 0)
{
	if (s_crash_callback == null) return;
	CrashReport cr;
	cr.code = code;
	cr.reason = reason;
	cr.callstack = s_callstack_pool;
	s_crash_callback(&cr);
}

#if __cplusplus
extern "C"
#endif
void crash_signal_handler(int sig)
{
	switch (sig)
	{
	case SIGTERM:	crash_call_callback(ECR_APPLICATION_TERMINATED); break;
	case SIGSEGV:	crash_call_callback(ECR_ACCESS_VIOLATION); break;
	case SIGINT:	crash_call_callback(ECR_EXTERNAL_INTERRUPT); break;
	case SIGILL:	crash_call_callback(ECR_INVALID_IMAGE); break;
	case SIGABRT:	crash_call_callback(ECR_ABNORMAL_TERMINATION); break;
	case SIGFPE:	crash_call_callback(ECR_ARITHMETIC_OPERATION); break;
	}
}

#if __cplusplus
extern "C"
#endif
void crash_unexpected_handler(void)
{
	crash_call_callback(ECR_UNKNOWN);
}

#if __cplusplus
extern "C"
#endif
void crash_pure_call_handler(void)
{
	crash_call_callback(ECR_PURE_CALL);
}

#if __cplusplus
extern "C"
#endif
void crash_invalid_parameter_handler(const wchar_t* expression, const wchar_t* functionName, const wchar_t* file, unsigned int line, uint pReserved)
{
#if SEGAN_CRASHRPT_CALLSTACK
	CrashCallStack* csd = callstack_push_pop(false);
	if (csd)
	{
		csd->file = (char*)"Somewhere in standard library.c";
		csd->line = 0;
		csd->func = null;
		sx_sprintf(csd->param, 256, "%S %S", functionName, expression);
	}
#endif
	crash_call_callback(ECR_INVALID_PARAMETER);
}

#if __cplusplus
extern "C"
#endif
int crash_new_operator_handler(size_t memsiz)
{
	crash_call_callback(ECR_MEMORY_ALLOC);
	return 0;
}

#if defined(_WIN32)
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
#endif

//	more features needed !!!
void sx_set_crash_handler(void)
{
#if defined(_WIN32)
	SetUnhandledExceptionFilter(crash_seh_handler);
	_set_error_mode(_OUT_TO_STDERR);

	_set_purecall_handler(crash_pure_call_handler);
	_set_invalid_parameter_handler(crash_invalid_parameter_handler);

	_set_new_mode(1);
	_set_new_handler(crash_new_operator_handler);

	set_unexpected(crash_unexpected_handler);
#endif

	signal(SIGTERM, crash_signal_handler);
	signal(SIGSEGV, crash_signal_handler);
	signal(SIGINT, crash_signal_handler);
	signal(SIGILL, crash_signal_handler);
	signal(SIGABRT, crash_signal_handler);
	signal(SIGFPE, crash_signal_handler);
}


//////////////////////////////////////////////////////////////////////////
//	interfaces
SEGAN_LIB_API CrashCallback crash_reporter_callback(CrashCallback callback)
{
	CrashCallback res = s_crash_callback;
	s_crash_callback = callback;
	return res;
}

SEGAN_LIB_API uint crash_reporter_install_process(void)
{
	//	more features needed !!!
	sx_set_crash_handler();
	return sx_process_currentId();
}

SEGAN_LIB_API uint crash_reporter_install_thread(void)
{
	//	more features needed !!!
	sx_set_crash_handler();
	return sx_thread_currentId();
}

SEGAN_LIB_API const char* crash_reporter_translate(ECrashReason reason)
{
	switch (reason)
	{
	case ECR_UNKNOWN:					return "Application crashes unexpectedly!";
	case ECR_MEMORY_ALLOC:				return "Can't allocate sufficient memory!";
	case ECR_APPLICATION_TERMINATED:	return "Application was terminated!";
	case ECR_ACCESS_VIOLATION:			return "Access Violation! The thread tried to read from or write to a virtual address for which it does not have the appropriate access.";
	case ECR_EXTERNAL_INTERRUPT:		return "Application has requested the Runtime to terminate it in an unusual way. Usually initiated by the user Ctrl+C.";
	case ECR_INVALID_IMAGE:				return "Invalid program image, such as invalid instruction!";
	case ECR_ABNORMAL_TERMINATION:		return "Abnormal termination condition, as is e.g. initiated by std::abort()!";
	case ECR_ARITHMETIC_OPERATION:		return "Erroneous arithmetic operation such as divide by zero!";
	case ECR_PURE_CALL:					return "The thread tried to call an abstracted uninitialized virtual function!";
	case ECR_INVALID_PARAMETER:			return "Call a function with invalid parameter!";
	case ECR_ARRAY_BOUNDS:				return "The thread tried to access an array element that is out of bounds and the underlying hardware supports bounds checking.";
	case ECR_DATATYPE_MISALIGNMENT:		return "The thread tried to read or write data that is misaligned on hardware that does not provide alignment. For example, 16-bit values must be aligned on 2-byte boundaries; 32-bit values on 4-byte boundaries, and so on.";
	case ECR_FLT_DENORMAL:				return "One of the operands in a floating-point operation is denormal. A denormal value is one that is too small to represent as a standard floating-point value.";
	case ECR_FLT_DIVIDE_BY_ZERO:		return "The thread tried to divide a floating-point value by a floating-point divisor of zero.";
	case ECR_FLT_INEXACT:				return "The result of a floating-point operation cannot be represented exactly as a decimal fraction.";
	case ECR_FLT_INVALID:				return "This exception represents any floating-point exception not recognized!";
	case ECR_FLT_OVERFLOW:				return "The exponent of a floating-point operation is greater than the magnitude allowed by the corresponding type.";
	case ECR_FLT_STACK_CHECK:			return "The stack overflowed or underflowed as the result of a floating-point operation.";
	case ECR_FLT_UNDERFLOW:				return "The exponent of a floating-point operation is less than the magnitude allowed by the corresponding type.";
	case ECR_ILLEGAL_INSTRUCTION:		return "The thread tried to execute an invalid instruction.";
	case ECR_INT_DIVIDE_BY_ZERO:		return "The thread tried to divide an integer value by an integer divisor of zero.";
	case ECR_INT_OVERFLOW:				return "The result of an integer operation caused a carry out of the most significant bit of the result.";
	case ECR_NONCONTINUABLE_EXCEPTION:	return "The thread tried to continue execution after a noncontinuable exception occurred.";
	case ECR_PRIV_INSTRUCTION:			return "The thread tried to execute an instruction whose operation is not allowed in the current machine mode.";
	case ECR_SINGLE_STEP:				return "A trace trap or other single-instruction mechanism signaled that one instruction has been executed.";
	case ECR_STACK_OVERFLOW:			return "The thread/process used up its stack. Stack overflow!";
	}
	return "Application crashes unexpectedly!";
}


#endif // SEGAN_CRASHRPT
