// containers.cpp : Defines the entry point for the console application.
//
#include "ImportLib.h"

#include <stdio.h>


int main(int argc, char* argv[])
{

    char * test = "VerificationServiceA09121234567";
    char res[65] = { 0 };
    sx_md5(res, test, null);
    sx_sha256(res, test, sx_str_len(test));
    printf("%s -> %s", test, res);
	getchar();

	return 0;
}

