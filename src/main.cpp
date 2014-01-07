/********************  HEADERS  *********************/
#include <execinfo.h>
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <dlfcn.h>
#include "json/TypeToJson.h"
#include "lib/SimpleStackTracer.h"

/*******************  FUNCTION  *********************/
void funcC(void)
{
	free(malloc(16));
}

/*******************  FUNCTION  *********************/
void funcB(void)
{
	free(malloc(32));
	funcC();
}

/*******************  FUNCTION  *********************/
void funcA(void)
{
	free(malloc(64));
	funcB();
}

/*******************  FUNCTION  *********************/
int main(int argc, char **argv)
{
	for (int i = 0 ; i < 10 ; i++)
	{
		free(calloc(16,16));
		free(malloc(16));
	}
	for (int i = 0 ; i < 4 ; i++)
		funcA();
	funcB();
	return 0;
}
