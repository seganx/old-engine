#include <cstdio>

#include "../../../sxLib/Trace.h"

void memreport(void* userdata, const char* file, const int line, const uint size, const bool corrupted)
{
    sx_trace();

    int* a = 0;
    *a = 1;

    if (corrupted)
        printf("%s(%d): error: memory corrupted %d b\n", file, line, size);
    else
        printf("%s(%d): warning: memory leak %d b\n", file, line, size);

    sx_return();
}

int main()
{
    printf("hello from Authenticator!\n");

    trace_attach(32, "trace_test.txt");
    sx_trace();

    memreport(null, __FILE__, __LINE__, 23, true);

    trace_detach();
    
    getchar();
    return 0;
}