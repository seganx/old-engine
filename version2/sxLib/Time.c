#include "Time.h"
#include "Platform.h"
#include <time.h>

SEGAN_LIB_API sx_time sx_time_now()
{
    return time(null);
}

SEGAN_LIB_API sx_time sx_time_diff(const sx_time t1, const sx_time t2)
{
    return difftime(t1, t2);
}

SEGAN_LIB_API void sx_time_print(char* dest, const uint destsize, const sx_time timeval)
{
    struct tm timeInfo;
    localtime_s(&timeInfo, &timeval);
    strftime(dest, destsize, "%Y-%m-%d %H:%M:%S", &timeInfo);
}
