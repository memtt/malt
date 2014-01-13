/********************  HEADERS  *********************/
#include <iostream>
#include <cstdio>
#include "pin.H"
#include "lib/AllocStackProfiler.h"

using namespace std;

/********************  GLOBALS  **********************/
#if defined(TARGET_MAC)
#define MALLOC "_malloc"
#define FREE "_free"
#else
#define MALLOC "malloc"
#define FREE "free"
#endif

/*********************  STRUCT  *********************/
struct ToolState
{
	//functions
	ToolState(void);
	//members
	AllocStackProfiler profiler;
	bool inUse;
};

/********************  GLOBALS  **********************/
static ToolState gblState;

/*******************  FUNCTION  *********************/
ToolState::ToolState(void )
	:profiler(STACK_MODE_ENTER_EXIT_FUNC)
{
	this->inUse = false;
}

/*******************  FUNCTION  *********************/
static VOID afterMalloc(ADDRINT ret,ADDRINT size)
{
	if (!gblState.inUse)
	{
		gblState.inUse = !gblState.inUse;
		gblState.profiler.onMalloc((void*)ret,size);
		gblState.inUse = !gblState.inUse;
	}
}

/*******************  FUNCTION  *********************/
static VOID beforeFree(ADDRINT ptr)
{
	if (!gblState.inUse)
	{
		gblState.inUse = !gblState.inUse;
		gblState.profiler.onFree((void*)ptr);
		gblState.inUse = !gblState.inUse;
	}
}

/*******************  FUNCTION  *********************/
static VOID instrImageMalloc(IMG img)
{
	// Instrument the malloc() and free() functions.  Print the input argument
	// of each malloc() or free(), and the return value of malloc().
	//
	//  Find the malloc() function.
	RTN mallocRtn = RTN_FindByName(img, MALLOC);
	if (RTN_Valid(mallocRtn))
    {
		RTN_Open(mallocRtn);
		
		// Instrument malloc() to print the input argument value and the return value.
		RTN_InsertCall(mallocRtn, IPOINT_AFTER, (AFUNPTR)afterMalloc,
                       IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
		               IARG_FUNCRET_EXITPOINT_VALUE,
		               IARG_END);
		RTN_Close(mallocRtn);
    }
}

/*******************  FUNCTION  *********************/
static VOID instrImageFree(IMG img)
{
	// Instrument the malloc() and free() functions.  Print the input argument
	// of each malloc() or free(), and the return value of malloc().
	//
	//  Find the malloc() function.
	RTN freeRtn = RTN_FindByName(img, FREE);
	if (RTN_Valid(freeRtn))
    {
		RTN_Open(freeRtn);

		// Instrument malloc() to print the input argument value and the return value.
		RTN_InsertCall(freeRtn, IPOINT_BEFORE, (AFUNPTR)beforeFree,
                       IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
		               IARG_END);
		RTN_Close(freeRtn);
    }
}

/*******************  FUNCTION  *********************/
void beforeFunc(void * fctAddr)
{
	gblState.profiler.onEnterFunction(fctAddr);
}

/*******************  FUNCTION  *********************/
void afterFunc(void * fctAddr)
{
	gblState.profiler.onExitFunction(fctAddr);
}

/*******************  FUNCTION  *********************/
VOID instrFunctions(RTN rtn, VOID *v)
{
	RTN_Open(rtn);

	void * addr =  (void*)RTN_Address(rtn);

	// Insert a call at the entry point of a routine to increment the call count
	RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)beforeFunc, IARG_PTR, addr, IARG_END);
	RTN_InsertCall(rtn, IPOINT_AFTER, (AFUNPTR)afterFunc, IARG_PTR, addr, IARG_END);

	RTN_Close(rtn);
}

/*******************  FUNCTION  *********************/
//Inspirate from code exemple of pintool doc
static VOID instrImage(IMG img, VOID *v)
{
	instrImageMalloc(img);
	instrImageFree(img);
}

/*******************  FUNCTION  *********************/
static VOID onExit(INT32 code, VOID *v)
{
	gblState.profiler.onExit();
}

/*******************  FUNCTION  *********************/
static INT32 usage()
{
	cerr << "This tool produces a trace of calls to malloc." << endl;
	cerr << endl << KNOB_BASE::StringKnobSummary() << endl;
	return -1;
}

/*******************  FUNCTION  *********************/
int main(int argc, char *argv[])
{
	// Initialize pin & symbol manager
	PIN_InitSymbols();
	if( PIN_Init(argc,argv) )
	{
		return usage();
	}

	// Register Image to be called to instrument functions.
	IMG_AddInstrumentFunction(instrImage, 0);
	RTN_AddInstrumentFunction(instrFunctions, 0);
	PIN_AddFiniFunction(onExit, 0);

	// Never returns
	PIN_StartProgram();

	return 0;
}
