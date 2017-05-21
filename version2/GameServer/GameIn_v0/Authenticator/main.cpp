#include <cstdio>

#include "../../../sxLib/Memory.h"
#include "../../../sxLib/Trace.h"

void memreport()
{
    sx_trace();

    int* b = (int*)sx_mem_alloc(4);
    b[6] = 4;

    b = 0;
    b[0] = 4;

    sx_return();
}

int main()
{
    printf("hello from Authenticator!\n");

    sx_trace_attach(32, null);
    sx_trace();

    int* a = (int*)sx_mem_alloc(16);
    a[4] = 0;

    memreport();

    sx_trace_detach();
    
    getchar();
    return 0;
}