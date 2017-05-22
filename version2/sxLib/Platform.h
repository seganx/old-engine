/********************************************************************
created:	2017/01/20
filename: 	Platform.h
Author:		Sajad Beigjani
eMail:		sajad.b@gmail.com
Site:		www.seganx.com
Desc:		This file contains some basic wrapper functions that are
            depend on operating system
*********************************************************************/
#ifndef HEADER_PLATFORM
#define HEADER_PLATFORM

//! includes
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#ifndef SEGAN_LIB_API
#define SEGAN_LIB_API
#endif

//////////////////////////////////////////////////////////////////////////
//	basic types
//////////////////////////////////////////////////////////////////////////
#ifndef uint
typedef unsigned int	uint;
#endif

struct sx_mutex;
struct sx_cond;
struct sx_semaphore;
struct sx_thread;
struct sx_threadpool;

typedef void (*sx_thread_func)(void *);


//////////////////////////////////////////////////////////////////////////
//	preprocess functions
//////////////////////////////////////////////////////////////////////////
#define sx_print(fmt, ...)						print(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

#if defined(_WIN32)
#define sx_fprintf(file, fmt, ...)				fprintf(file, fmt, ##__VA_ARGS__)
#define sx_sprintf(dest, len, fmt, ...)			snprintf(dest, len, fmt, ##__VA_ARGS__)

#define sx_vsprintf_len(fmt, args)				(vsnprintf(0, 0, fmt, args) + 1)
#define sx_vsprintf(dest, len, fmt, args)		vsnprintf(dest, len, fmt, args)

#define RED   
#define GRN   
#define YEL   
#define BLU   
#define MAG   
#define CYN   
#define WHT   
#define RESET 


#else
#define sx_fprintf(file, fmt, ...)				fprintf(file, fmt, ##__VA_ARGS__)
#define sx_sprintf(dest, len, fmt, ...)			snprintf(dest, len, fmt, ##__VA_ARGS__)

#define sx_vsprintf_len(fmt, args)				(vsnprintf(0, 0, fmt, args) + 1)
#define sx_vsprintf(dest, len, fmt, args)		vsnprintf(dest, len, fmt, args)

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

#endif


//////////////////////////////////////////////////////////////////////////
//	basic functions
//////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

SEGAN_LIB_API int print(const char* filename, const int line, const char* format, ...);

SEGAN_LIB_API struct sx_mutex * sx_mutex_create();
SEGAN_LIB_API int sx_mutex_destroy(struct sx_mutex * mutex);
SEGAN_LIB_API int sx_mutex_lock(struct sx_mutex * mutex);
SEGAN_LIB_API int sx_mutex_unlock(struct sx_mutex * mutex);

SEGAN_LIB_API struct sx_cond * sx_cond_create();
SEGAN_LIB_API int sx_cond_destroy(struct sx_cond * cond);
SEGAN_LIB_API int sx_cond_wait(struct sx_cond * cond, struct sx_mutex * mutex);
SEGAN_LIB_API int sx_cond_signal(struct sx_cond * cond);
SEGAN_LIB_API int sx_cond_broadcast(struct sx_cond * cond);

SEGAN_LIB_API struct sx_semaphore * sx_semaphore_create(const uint init_count, const uint max_count);
SEGAN_LIB_API int sx_semaphore_destroy(struct sx_semaphore * semaphore);
SEGAN_LIB_API int sx_semaphore_wait(struct sx_semaphore * semaphore);
SEGAN_LIB_API int sx_semaphore_post(struct sx_semaphore * semaphore);
SEGAN_LIB_API int sx_semaphore_getval(struct sx_semaphore * semaphore);

SEGAN_LIB_API struct sx_thread * sx_thread_create(const uint id, sx_thread_func func, void * param);
SEGAN_LIB_API int sx_thread_destroy(struct sx_thread * thread);
SEGAN_LIB_API uint sx_thread_id(struct sx_thread * thread);

SEGAN_LIB_API struct sx_threadpool* sx_threadpool_create(const uint thread_count);
SEGAN_LIB_API int sx_threadpool_destroy(struct sx_threadpool * threadpool);
SEGAN_LIB_API int sx_threadpool_add_job(struct sx_threadpool * threadpool, sx_thread_func func, void * param);
SEGAN_LIB_API uint sx_threadpool_num_jobs(struct sx_threadpool * threadpool);
SEGAN_LIB_API uint sx_threadpool_num_busy_threads(struct sx_threadpool * threadpool);

SEGAN_LIB_API void sx_sleep(const uint miliseconds);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // HEADER_PLATFORM
