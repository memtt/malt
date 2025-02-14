#include <cstdio>
#include <Python.h>
//#include <thread>
//libc POSIX.1, here we use GNU specific RTLD_NEXT (need _GNU_SOURCE)
#include <dlfcn.h>
#include "../lib/wrapper/InjectPythonInit.hpp"

int main(int argc, char** argv)
{
	return Py_BytesMain(argc, argv);
}
