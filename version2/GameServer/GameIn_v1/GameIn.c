#include "GameIn.h"

int main(int argc, char* argv[])
{
	sx_trace_attach(10, "crash report.txt");
	sx_trace();

	char t[64] = {0};
    sx_time_print(t, 64, sx_time_now());
    sx_print(t);

    getchar();
	return 0;
}

