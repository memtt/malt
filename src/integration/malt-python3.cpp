#include <cstdio>
#include <Python.h>
//#include <thread>
//libc POSIX.1, here we use GNU specific RTLD_NEXT (need _GNU_SOURCE)
#include <dlfcn.h>

namespace MALT
{
	void initPythonAllocInstrumentation();
	typedef int (*Py_RunMainFuncPtr) (void);
	static Py_RunMainFuncPtr gblRealPy_RunMain = nullptr;
}

int Py_RunMain()
{
	MALT::initPythonAllocInstrumentation();
	return MALT::gblRealPy_RunMain();
}

int main(int argc, char** argv)
{
	MALT::gblRealPy_RunMain = (MALT::Py_RunMainFuncPtr)dlsym(RTLD_NEXT,"Py_RunMain");
	return Py_BytesMain(argc, argv);
}
