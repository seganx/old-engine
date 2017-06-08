#include "Trace.h"
#include "Platform.h"

#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#endif
#include <conio.h>

//! mutex object
typedef struct sx_mutex
{
#if defined(_WIN32)
    HANDLE  obj;
#else
    pthread_mutex_t obj;
#endif
} sx_mutex;

//! condition object
typedef struct sx_cond
{
#if defined(_WIN32)
    HANDLE signal, broadcast;
#else
    pthread_cond_t obj;
#endif
} sx_cond;

//! semaphore object
typedef struct sx_semaphore
{
#if defined(_WIN32)
    HANDLE  obj;
#else
    sem_t   obj;
#endif
} sx_semaphore;


//! thread object
typedef struct sx_thread
{
    uint            id;
    sx_thread_func  func;
    void*           param;

#if defined(_WIN32)
    HANDLE          thread;
#else
    pthread_t       thread;
#endif
} sx_thread;



//////////////////////////////////////////////////////////////////////////////////
//  thread pool
//////////////////////////////////////////////////////////////////////////////////

// threadpool job object
typedef struct threadpool_job
{
    struct threadpool_job*  prev;
    sx_thread_func          func;
    void*                   param;
}
threadpool_job;

// threadpool Job queue
typedef struct threadpool_jobqueue
{
    sx_mutex        mutex;
    threadpool_job* front;
    threadpool_job* rear;
    volatile uint   count;
}
threadpool_jobqueue;

//! threadpool object
typedef struct sx_threadpool
{
    const char*         trace_file;             //  filename for trace
    sx_thread*          threads;                //  array of thread objecs
    sx_semaphore        semaphore;              //  semaphore used to signal threads in pool
    threadpool_jobqueue jobqueue;               //  multi-threaded safe queue of jobs
    sx_mutex            mutex;                  //  used to write on integere params
    volatile uint       running;                //  indicates that thread pool is running
    volatile uint       num_threads_ready;      //  number of created threads which are ready
    volatile uint       num_threads_working;    //  number of threads which are working on jobs
}
sx_threadpool;



//////////////////////////////////////////////////////////////////////////////////
//  implementation
//////////////////////////////////////////////////////////////////////////////////
static const char* str_get_filename(const char* filename)
{
    const char* res = filename;
    for (const char* c = filename; *c != 0; ++c)
        if (*c == '/' || *c == '\\')
            res = c + 1;
    return res;
}

SEGAN_LIB_API int print(const char * filename, const int line, const char * format, ...)
{
    if (!format) return 0;

    va_list args;
    va_start(args, format);
    int res = vprintf(format, args);
    va_end(args);
    res += printf(" - %s(%d)\n", str_get_filename(filename), line);

    return res;;
}

static int sx_mutex_init(struct sx_mutex* mutex)
{
#if defined(_WIN32)
    mutex->obj = CreateMutex(NULL, FALSE, NULL);
    if (mutex->obj == NULL)
#else
    if (pthread_mutex_init(&mutex->obj, NULL) != 0)
#endif
        return -1;
    else
        return 0;
}

static int sx_mutex_finit(struct sx_mutex* mutex)
{
#if defined(_WIN32)
    return CloseHandle(mutex->obj) == 0 ? -1 : 0;
#else
    return pthread_mutex_destroy(&mutex->obj);
#endif
}

SEGAN_LIB_API struct sx_mutex * sx_mutex_create()
{
    struct sx_mutex * res = (struct sx_mutex *)malloc(sizeof(struct sx_mutex));

    if (res == NULL || sx_mutex_init(res) != 0)
    {
        sx_print("Error: Mutex initialization failed!");
        free(res);
        res = NULL;
    }

    return res;
}

SEGAN_LIB_API int sx_mutex_destroy(struct sx_mutex * mutex)
{
    if (!mutex) return 0;
    int res = sx_mutex_finit(mutex);
    free(mutex);
    return res;
}

SEGAN_LIB_API int sx_mutex_lock(struct sx_mutex * mutex)
{
#if defined(_WIN32)
    return WaitForSingleObject(mutex->obj, INFINITE) == WAIT_OBJECT_0 ? 0 : -1;
#else
    return pthread_mutex_lock(&mutex->obj);
#endif
}

SEGAN_LIB_API int sx_mutex_unlock(struct sx_mutex * mutex)
{
#if defined(_WIN32)
    return ReleaseMutex(mutex->obj) == 0 ? -1 : 0;
#else
    return pthread_mutex_unlock(&mutex->obj);
#endif
}

SEGAN_LIB_API struct sx_cond * sx_cond_create()
{
    struct sx_cond * res = (struct sx_cond *)malloc(sizeof(struct sx_cond));

#if defined(_WIN32)
    res->signal = CreateEvent(NULL, FALSE, FALSE, NULL);
    res->broadcast = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!res->signal || !res->broadcast)
#else
    if (pthread_cond_init(&res->obj, NULL) != 0)
#endif
    {
        sx_print("Error: Condition initialization failed!");
        free(res);
        res = NULL;
    }

    return res;
}

SEGAN_LIB_API int sx_cond_destroy(struct sx_cond * cond)
{
    if (!cond) return 0;

#if defined(_WIN32)
    int res = CloseHandle(cond->signal) && CloseHandle(cond->broadcast) ? 0 : -1;
#else
    int res = pthread_cond_destroy(&cond->obj);
#endif

    free(cond);
    return res;
}

SEGAN_LIB_API int sx_cond_wait(struct sx_cond * cond, struct sx_mutex * mutex)
{
#if defined(_WIN32)
    HANDLE handles[] = { cond->signal, cond->broadcast };
    ReleaseMutex(mutex->obj);
    WaitForMultipleObjects(2, handles, FALSE, INFINITE);
    return WaitForSingleObject(mutex->obj, INFINITE) == WAIT_OBJECT_0 ? 0 : -1;
#else
    return pthread_cond_wait(&cond->obj, &mutex->obj);
#endif
}

SEGAN_LIB_API int sx_cond_signal(struct sx_cond * cond)
{
#if defined(_WIN32)
    return SetEvent(cond->signal) == 0 ? -1 : 0;
#else
    return pthread_cond_signal(&cond->obj);
#endif
}

SEGAN_LIB_API int sx_cond_broadcast(struct sx_cond * cond)
{
#if defined(_WIN32)
    // Implementation with PulseEvent() has race condition, see
    // http://www.cs.wustl.edu/~schmidt/win32-cv-1.html
    return PulseEvent(cond->broadcast) == 0 ? -1 : 0;
#else
    return pthread_cond_broadcast(&cond->obj);
#endif
}


static int sx_semaphore_init(struct sx_semaphore* semaphore, const uint init_count, const uint max_count)
{
#if defined(_WIN32)
    semaphore->obj = CreateSemaphore(NULL, init_count, max_count, NULL);
    if (semaphore->obj == NULL)
#else
    if (sem_init(&semaphore->obj, 0, init_count) != 0)
#endif
        return -1;
    else
        return 0;
}

static int sx_semaphore_finit(struct sx_semaphore* semaphore)
{
#if defined(_WIN32)
    return CloseHandle(semaphore->obj) == 0 ? -1 : 0;
#else
    return sem_destroy(&semaphore->obj);
#endif
}

SEGAN_LIB_API struct sx_semaphore * sx_semaphore_create(const uint init_count, const uint max_count)
{
    struct sx_semaphore * res = (struct sx_semaphore*)malloc(sizeof(struct sx_semaphore));

    if (res == NULL || sx_semaphore_init(res, init_count, max_count) != 0)
    {
        sx_print("Error: Semaphore initialization failed!");
        free(res);
        res = NULL;
    }

    return res;
}

SEGAN_LIB_API int sx_semaphore_destroy(struct sx_semaphore * semaphore)
{
    if (!semaphore) return 0;
    int res = sx_semaphore_finit(semaphore);
    free(semaphore);
    return res;
}

SEGAN_LIB_API int sx_semaphore_wait(struct sx_semaphore * semaphore)
{
#if defined(_WIN32)
    return WaitForSingleObject(semaphore->obj, INFINITE) == WAIT_OBJECT_0 ? 0 : -1;
#else
    return sem_wait(&semaphore->obj);
#endif
}

SEGAN_LIB_API int sx_semaphore_post(struct sx_semaphore * semaphore)
{
#if defined(_WIN32)
    return ReleaseSemaphore(semaphore->obj, 1, NULL) ? 0 : -1;
#else
    return sem_post(&semaphore->obj);
#endif
}

SEGAN_LIB_API int sx_semaphore_getval(struct sx_semaphore * semaphore)
{
#if defined(_WIN32)
    //long res = 0;
    //return ReleaseSemaphore(semaphore->obj, 0, &res) ? (int)res : -1;
    return -1;
#else
    int res = -1;
    if (sem_getvalue(&semaphore->obj, &res) != 0)
        return -1;
    return res;
#endif
}



#if defined(_WIN32)
DWORD WINAPI
#else
void *
#endif
sx_thread_worker_internal(void* p) {
    struct sx_thread * thread = (struct sx_thread *)p;
    thread->func(thread->param);
#if defined(_WIN32)
    return 0;
#else
    return NULL;
#endif
}

static int sx_thread_init(struct sx_thread * thread, const uint id, sx_thread_func func, void * param)
{
    thread->id = id;
    thread->func = func;
    thread->param = param;
#if defined(_WIN32)
    thread->thread = CreateThread(NULL, 0, sx_thread_worker_internal, thread, 0, NULL);
    if (thread->thread == NULL)
        return -1;
#else
    if (pthread_create(&thread->thread, NULL, sx_thread_worker_internal, thread) != 0)
        return -1;
    else
        pthread_detach(thread->thread);
#endif
    return 0;
}

static int sx_thread_finit(struct sx_thread * thread)
{
#if defined(_WIN32)
    return TerminateThread(thread->thread, 0) == 0 ? 1 : 0;
#else
    return 0;// pthread_cancel(thread->thread);
#endif
}


SEGAN_LIB_API struct sx_thread * sx_thread_create(const uint id, sx_thread_func func, void * param)
{
    struct sx_thread * res = (struct sx_thread *)malloc(sizeof(struct sx_thread));
    if (res == NULL || sx_thread_init(res, id, func, param) != 0)
    {
        sx_print("Error: Failed to create thread!");
        free(res);
        res = NULL;
    }
    return res;
}

SEGAN_LIB_API int sx_thread_destroy(struct sx_thread * thread)
{
    if (!thread) return 0;
    int res = sx_thread_finit(thread);
    free(thread);
    return res;
}

SEGAN_LIB_API uint sx_thread_id(struct sx_thread * thread)
{
    if (!thread)
#if defined(_WIN32)
        return (uint)GetCurrentThreadId();
#else
        return (uint)pthread_self();
#endif
    else return thread->id;
}



//////////////////////////////////////////////////////////////////////////////////
//  thread pool implementation
//////////////////////////////////////////////////////////////////////////////////

static int jobqueue_init(struct threadpool_jobqueue* jobqueue)
{
    jobqueue->front = NULL;
    jobqueue->rear = NULL;
    jobqueue->count = 0;
    return sx_mutex_init(&jobqueue->mutex);
}

static void jobqueue_queue(struct threadpool_jobqueue* jobqueue, struct threadpool_job* job)
{
    job->prev = NULL;

    if (jobqueue->count)
        jobqueue->rear = jobqueue->rear->prev = job;
    else
        jobqueue->front = jobqueue->rear = job;

    jobqueue->count++;
}

static struct threadpool_job* jobqueue_dequeue(struct threadpool_jobqueue* jobqueue)
{
    struct threadpool_job* res = jobqueue->front;

    switch (jobqueue->count)
    {
    case 0: break;

    case 1: jobqueue->front = jobqueue->rear = NULL;
        jobqueue->count = 0; 
        break;

    default:
        jobqueue->front = res->prev;
        jobqueue->count--;
        break;
    }

    return res;
}

static void jobqueue_clear(struct threadpool_jobqueue* jobqueue)
{
    while (jobqueue->count)
        free(jobqueue_dequeue(jobqueue));

    jobqueue->front = NULL;
    jobqueue->rear = NULL;
    jobqueue->count = 0;
}

static int jobqueue_destroy(struct threadpool_jobqueue* jobqueue)
{
    jobqueue_clear(jobqueue);
    return sx_mutex_finit(&jobqueue->mutex);
}

static void threadpool_worker(void* p)
{
    struct sx_threadpool* threadpool = (struct sx_threadpool*)p;
    sx_trace_attach(10, threadpool->trace_file);

    // count number of threads which are ready
    sx_mutex_lock(&threadpool->mutex);
    threadpool->num_threads_ready++;
    sx_mutex_unlock(&threadpool->mutex);

    while (threadpool->running)
    {
        //  wait to receive a signal from semaphore
        sx_semaphore_wait(&threadpool->semaphore);

        //  if thread is going to be off just break the loop
        if (!threadpool->running) break;

        //  increase number of working threads
        sx_mutex_lock(&threadpool->mutex);
        threadpool->num_threads_working++;
        sx_mutex_unlock(&threadpool->mutex);

        // dequeue job from jobs queue
        struct threadpool_job* job = null;
        sx_mutex_lock(&threadpool->jobqueue.mutex);
        job = jobqueue_dequeue(&threadpool->jobqueue);
        sx_mutex_unlock(&threadpool->jobqueue.mutex);
        if (job)
        {
            sx_thread_func func = job->func;
            void* param = job->param;
            free(job);

            //  execute hte job
            func(param);

            //  release a thread to pop another job
            sx_semaphore_post(&threadpool->semaphore);
        }

        //  decrease number of working threads
        sx_mutex_lock(&threadpool->mutex);
        threadpool->num_threads_working--;
        sx_mutex_unlock(&threadpool->mutex);
    }

    // count down number of threads which are ready
    sx_mutex_lock(&threadpool->mutex);
    threadpool->num_threads_ready--;
    sx_mutex_unlock(&threadpool->mutex);

    sx_trace_detach();
}


SEGAN_LIB_API struct sx_threadpool* sx_threadpool_create(const uint num_threads, const char* trace_filename)
{
    if (num_threads <= 0)
        return NULL;

    struct sx_threadpool* res = (struct sx_threadpool*)malloc(sizeof(struct sx_threadpool));
    if (res == NULL)
    {
        sx_print("Error: Can't allocate memory for thread pool!\n");
        return NULL;
    }
    res->trace_file = trace_filename;

    res->threads = (struct sx_thread*)calloc(num_threads, sizeof(struct sx_thread));
    if (res->threads == NULL)
    {
        sx_print("Error: Can't allocate memory for thread pool!\n");
        free(res);
        return NULL;
    }

    res->running = 1;
    res->num_threads_ready = 0;
    res->num_threads_working = 0;

    if (jobqueue_init(&res->jobqueue) != 0)
    {
        free(res->threads);
        free(res);
        return NULL;
    }

    if (sx_mutex_init(&res->mutex) != 0)
    {
        jobqueue_destroy(&res->jobqueue);
        free(res->threads);
        free(res);
        return NULL;
    }

    if (sx_semaphore_init(&res->semaphore, 0, 1) != 0)
    {
        jobqueue_destroy(&res->jobqueue);
        sx_mutex_finit(&res->mutex);
        free(res->threads);
        free(res);
        return NULL;
    }

    for (uint i = 0; i < num_threads; ++i)
        sx_thread_init(&res->threads[i], i, threadpool_worker, res);

    // wait for threads to be initialized
    while (res->num_threads_ready != num_threads) {}

    return res;
}

SEGAN_LIB_API int sx_threadpool_destroy(sx_threadpool * threadpool)
{
    if (threadpool == NULL) return 0;

    volatile uint num_threads = threadpool->num_threads_ready;

    //  wait for working threads to be finished
    while (threadpool->num_threads_working)
        sx_sleep(1);

    // finish threads infinite loop
    threadpool->running = 0;

    // release remaining threads to be finished
    while (threadpool->num_threads_ready)
    {
        sx_semaphore_post(&threadpool->semaphore);
        sx_sleep(1);
    }

    // cleanup everything
    sx_semaphore_finit(&threadpool->semaphore);
    sx_mutex_finit(&threadpool->mutex);
    jobqueue_destroy(&threadpool->jobqueue);

    for (uint i = 0; i < num_threads; ++i)
        sx_thread_finit(&threadpool->threads[i]);

    free(threadpool->threads);
    free(threadpool);

    return 0;
}

SEGAN_LIB_API int sx_threadpool_add_job(struct sx_threadpool* threadpool, sx_thread_func func, void * param)
{
    struct threadpool_job* job = (struct threadpool_job*)malloc(sizeof(struct threadpool_job));
    if (job == NULL) 
    {
        sx_print("Error: Can't allocate memory for thread pool job!\n");
        return -1;
    }

    job->prev = null;
    job->func = func;
    job->param = param;

    sx_mutex_lock(&threadpool->jobqueue.mutex);
    jobqueue_queue(&threadpool->jobqueue, job);
    sx_mutex_unlock(&threadpool->jobqueue.mutex);

    //  release a thread to handle new job
    return sx_semaphore_post(&threadpool->semaphore);
}

SEGAN_LIB_API uint sx_threadpool_num_jobs(struct sx_threadpool * threadpool)
{
    return threadpool->jobqueue.count;
}

SEGAN_LIB_API uint sx_threadpool_num_busy_threads(struct sx_threadpool * threadpool)
{
    return threadpool->num_threads_working;
}


SEGAN_LIB_API void sx_sleep(const uint miliseconds)
{
#if defined(_WIN32)
    Sleep(miliseconds);
#else
    usleep(miliseconds * 1000);
#endif	
}

SEGAN_LIB_API char sx_getch()
{
    char r = 0;
    if (_kbhit())
        r = _getch();
    return r;
}

