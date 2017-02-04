/********************************************************************
	created:	2015/12/28
	filename: 	Crash.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain some macro and functions for 
				crash handler system
*********************************************************************/
#ifndef HEADER_FILE_CRASH
#define HEADER_FILE_CRASH

#include "Def.h"

#if SEGAN_CRASHRPT

enum crash_reason
{
	ECR_UNKNOWN = 0,
	ECR_MEMORY_ALLOC,
	ECR_APPLICATION_TERMINATED,
	ECR_ACCESS_VIOLATION,
	ECR_EXTERNAL_INTERRUPT,
	ECR_INVALID_IMAGE,
	ECR_ABNORMAL_TERMINATION,
	ECR_ARITHMETIC_OPERATION,
	ECR_PURE_CALL,
	ECR_INVALID_PARAMETER,
	ECR_ARRAY_BOUNDS,
	ECR_DATATYPE_MISALIGNMENT,			// for example, 16-bit values must be aligned on 2-byte boundaries; 32-bit values on 4-byte boundaries, and so on.
	ECR_FLT_DENORMAL,					// one of the operands in a floating-point operation is denormal. A denormal value is one that is too small to represent as a standard floating-point value.
	ECR_FLT_DIVIDE_BY_ZERO,			
	ECR_FLT_INEXACT,					// the result of a floating-point operation cannot be represented exactly as a decimal fraction.
	ECR_FLT_INVALID,					// this exception represents any floating-point exception not included in this list.
	ECR_FLT_OVERFLOW,					// the exponent of a floating-point operation is greater than the magnitude allowed by the corresponding type.
	ECR_FLT_STACK_CHECK,				// the stack overflowed or underflowed as the result of a floating-point operation.
	ECR_FLT_UNDERFLOW,					// the exponent of a floating-point operation is less than the magnitude allowed by the corresponding type.
	ECR_ILLEGAL_INSTRUCTION,			// the thread tried to execute an invalid instruction.
	ECR_INT_DIVIDE_BY_ZERO,				// the thread tried to divide an integer value by an integer divisor of zero.
	ECR_INT_OVERFLOW,					// the result of an integer operation caused a carry out of the most significant bit of the result.
	ECR_NONCONTINUABLE_EXCEPTION,		// the thread tried to continue execution after a non-continuable exception occurred.
	ECR_PRIV_INSTRUCTION,				// the thread tried to execute an instruction whose operation is not allowed in the current machine mode.
	ECR_SINGLE_STEP,					// a trace trap or other single-instruction mechanism signaled that one instruction has been executed.
	ECR_STACK_OVERFLOW,					// the thread used up its stack.
};

#if SEGAN_CRASHRPT_CALLSTACK
//! call stack structure
struct CrashCallStack
{
	char	param[256];	//	function parameters
	char*	func;		//	function name
	char*	file;
	sint	line;
};
#endif

//! crash report structure
struct CrashReport 
{
	dword code;
	crash_reason reason;

#if SEGAN_CRASHRPT_CALLSTACK
	CrashCallStack* callstack;	// array of call-stack ended with cleaned one
#endif
};

#if SEGAN_CRASHRPT_CALLSTACK
//! we used c++ class construct/destruct behavior to push/pop calls on stack
class SEGAN_LIB_API _CallStack
{
public:
	_CallStack( const char* file, const sint line, const char* functionName );
	_CallStack( const sint line, const char* file, const char* functionName, ... );
	~_CallStack( void );
};
#endif

//! callback function will call when application crashes
typedef void (* CrashCallback)(CrashReport* crashreport);

SEGAN_LIB_API CrashCallback crash_reporter_callback(CrashCallback callback);
SEGAN_LIB_API uint crash_reporter_install_process( void );
SEGAN_LIB_API uint crash_reporter_install_thread( void );
SEGAN_LIB_API const char* crash_reporter_translate( crash_reason reason );

#if SEGAN_CRASHRPT_CALLSTACK
//! create a new call stack for function with out parameters
#define sx_callstack()						_CallStack sx_unique_name(callstack)( __FILE__, __LINE__, __FUNCTION__ );

//! create new call stack for function with name and parameters
#define sx_callstack_param(function,...)	_CallStack sx_unique_name(callstack)( __LINE__, __FILE__, #function, __VA_ARGS__ );
#else
#define sx_callstack()
#define sx_callstack_param(function,...)
#endif

//! set callback function for crash handler and return pointer to previous callback function
#define sx_crash_callback(callback)			crash_reporter_callback(callback)

//! install crash handler on the current process and return process id. return 0 on fail
#define sx_crash_handler_process()			crash_reporter_install_process()

//! install crash handler on the current thread and return thread id. return 0 on fail
#define sx_crash_handler_thread()			crash_reporter_install_thread()

//! translate crash reason to the human readable text
#define sx_crash_translate(reason)			crash_reporter_translate(reason)

#else
#define sx_callstack()
#define sx_callstack_param(function,...)
#define sx_crash_callback(callback)
#define sx_crash_handler_process()
#define sx_crash_handler_thread()
#define sx_crash_translate(reason)
#endif

#endif	//	HEADER_FILE_CRASH
