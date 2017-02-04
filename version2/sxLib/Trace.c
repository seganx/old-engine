#if defined(_WIN32)
#include <Windows.h>
#endif

#include "Trace.h"


//////////////////////////////////////////////////////////////////////////
//	simple call stack system
//////////////////////////////////////////////////////////////////////////
#if (SEGANX_TRACE_CALLSTACK || SEGANX_TRACE_PROFILER)
typedef struct trace_info
{
    char*	func;		    //	function name
    char*	file;           //  file name
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

#if (SEGANX_TRACE_CALLSTACK || SEGANX_TRACE_PROFILER)
typedef struct trace_object
{
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

#if 0
static SEGAN_INLINE void trace_info_clear( struct trace_info * tinfo )
{
    tinfo->func = null;
    tinfo->file = null;
    tinfo->line = 0;
    tinfo->time = 0;
    tinfo->param[0] = 0;
}

static void trace_object_clear( struct trace_object * tobj )
{
    for (int i = 0; i < tobj->callstack_count; ++i)
        trace_info_clear( &tobj->callstack_array[i] );
}
#endif

static void trace_object_push( struct trace_object* tobj )
{
    --tobj->callstack_index;
}

static struct trace_info * trace_object_pop( struct trace_object* tobj )
{
    return &tobj->callstack_array[tobj->callstack_index++];
}

SEGAN_LIB_API void trace_attach( uint stack_size )
{
    if ( s_current_object != null ) return;
    s_current_object = (struct trace_object*)calloc(1, sizeof(struct trace_object));
    s_current_object->callstack_count = stack_size;
    s_current_object->callstack_array = (struct trace_info*)calloc(stack_size, sizeof(struct trace_info*));
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
    if (tinfo)
    {
        tinfo->file = (char*)file;
        tinfo->line = line;
        tinfo->func = (char*)function;
        tinfo->param[0] = 0;
    }
}

SEGAN_LIB_API void trace_push_param( const char * file, const int line, const char * function, ... )
{
    struct trace_info* tinfo = trace_object_pop(s_current_object);
    if (tinfo)
    {
        tinfo->file = (char*)file;
        tinfo->line = line;
        tinfo->func = null;
        if (function)
        {
            va_list args;
            va_start(args, function);
            sint len = vsnprintf(0, 0, function, args) + 1;
            if (len < 128)
            {
                vsnprintf(tinfo->param, 128, function, args);
            }
            else
            {
                uint i = 0;
                for (; i < 128 && function[i]; ++i)
                    tinfo->param[i] = function[i];
                tinfo->param[i] = 0;
            }
            va_end(args);
        }
        else tinfo->param[0] = 0;
    }
}

SEGAN_LIB_API void trace_pop( void )
{
    trace_object_push(s_current_object);
}


#endif // SEGAN_CRASHRPT_CALLSTACK



