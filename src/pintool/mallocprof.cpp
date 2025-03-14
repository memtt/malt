/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.6
*    DATE     : 09/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/pintool/mallocprof.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (ECR) - 2014
*    AUTHOR   : Sébastien Valat - 2020 - 2024
***********************************************************/

/**********************************************************/
#include <iostream>
#include <cstdio>
#include <json/ConvertToJson.h>
#include "pin.H"
#include "lib/AllocStackProfiler.h"

using namespace std;

/**********************************************************/
#if defined(TARGET_MAC)
#define MALLOC "_malloc"
#define CALLOC "_calloc"
#define FREE "_free"
#else
#define MALLOC "malloc"
#define CALLOC "calloc"
#define FREE "free"
#endif

/**********************************************************/
struct ToolState
{
	//functions
	ToolState(void);
	//members
	AllocStackProfiler profiler;
	//need to TLS
	bool inUse;
	size_t lastMallocSize;	
	size_t lastCallocNMemb;
	FuncNameDic names;
	int depth;
};

/**********************************************************/
static ToolState gblState;

/**********************************************************/
ToolState::ToolState(void )
	:profiler(STACK_MODE_ENTER_EXIT_FUNC)
{
	this->inUse = false;
	this->lastMallocSize = 0;
	this->depth = 0;
}

/**********************************************************/
static VOID beforeMalloc(ADDRINT size)
{
	if (!gblState.inUse)
	{
		gblState.lastMallocSize = size;
	}
}

/**********************************************************/
static VOID afterMalloc(ADDRINT ret)
{
	if (!gblState.inUse)
	{
		gblState.inUse = true;
// 		fprintf(stderr,"%p = malloc(%zu)\n",(void*)ret,gblState.lastMallocSize);
		gblState.profiler.onMalloc((void*)ret,gblState.lastMallocSize);
		gblState.inUse = false;
	} else {
		puts("INUSE");
	}
}

/**********************************************************/
static VOID beforeCalloc(ADDRINT nmemb,ADDRINT size)
{
	if (!gblState.lastCallocNMemb)
	{
		gblState.lastMallocSize = size;
		gblState.lastCallocNMemb = size;
	}
}

/**********************************************************/
static VOID afterCalloc(ADDRINT ret)
{
	if (!gblState.inUse)
	{
		gblState.inUse = true;
		gblState.profiler.onCalloc((void*)ret,gblState.lastCallocNMemb,gblState.lastMallocSize);
		gblState.inUse = false;
	} else {
		puts("INUSE");
	}
}

/**********************************************************/
static VOID beforeFree(ADDRINT ptr)
{
	if (!gblState.inUse)
	{
		gblState.inUse = true;
// 		fprintf(stderr,"free(%p) [%zu]\n",(void*)ptr,sizeof(ADDRINT));
		gblState.profiler.onFree((void*)ptr);
		gblState.inUse = false;
	} else {
		puts("INUSE");
	}
}

/**********************************************************/
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
		RTN_InsertCall(mallocRtn, IPOINT_BEFORE, (AFUNPTR)beforeMalloc,
                       IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
		               IARG_END);
		
		// Instrument malloc() to print the input argument value and the return value.
		RTN_InsertCall(mallocRtn, IPOINT_AFTER, (AFUNPTR)afterMalloc,
					   IARG_FUNCRET_EXITPOINT_VALUE,
		               IARG_END);
		RTN_Close(mallocRtn);
    }
}

/**********************************************************/
static VOID instrImageCalloc(IMG img)
{
	// Instrument the malloc() and free() functions.  Print the input argument
	// of each malloc() or free(), and the return value of malloc().
	//
	//  Find the malloc() function.
	RTN callocRtn = RTN_FindByName(img, CALLOC);
	if (RTN_Valid(callocRtn))
    {
		RTN_Open(callocRtn);
		
		// Instrument malloc() to print the input argument value and the return value.
		RTN_InsertCall(callocRtn, IPOINT_AFTER, (AFUNPTR)beforeCalloc,
                       IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
		               IARG_FUNCARG_ENTRYPOINT_VALUE, 1,
		               IARG_END);

		// Instrument malloc() to print the input argument value and the return value.
		RTN_InsertCall(callocRtn, IPOINT_AFTER, (AFUNPTR)afterCalloc,
					   IARG_FUNCRET_EXITPOINT_VALUE,
		               IARG_END);
		RTN_Close(callocRtn);
    }
}

/**********************************************************/
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

/**********************************************************/
void beforeFunc(void * fctAddr)
{
// 	gblState.depth++;
	gblState.profiler.onEnterFunction(fctAddr);
// 	for (int i = 0 ; i < gblState.depth ; i++)
// 		printf(" ");
// 	printf("[%d] Enter %p = %s\n",gblState.depth,fctAddr,gblState.names.getName(fctAddr));
}

/**********************************************************/
void beforeFuncPrint(void * fctAddr)
{
	for (int i = 0 ; i < gblState.depth ; i++)
		printf(" ");
	printf("[%d] Enter2 %p = %s\n",gblState.depth,fctAddr,gblState.names.getName(fctAddr));
}

/**********************************************************/
void afterFunc(void * fctAddr)
{
// 	for (int i = 0 ; i < gblState.depth ; i++)
// 		printf(" ");
// 	printf("[%d] Exit %p = %s\n",gblState.depth,fctAddr,gblState.names.getName(fctAddr));
	gblState.profiler.onExitFunction(fctAddr);
// 	gblState.depth--;
}

/**********************************************************/
VOID instrFunctions(RTN rtn, VOID *v)
{
	RTN_Open(rtn);

	void * addr =  (void*)RTN_Address(rtn);
	string name = RTN_Name(rtn);
	
// 	if (name != ".text" && name != ".plt" && name != "__cxa_atexit" && name != "__cxa_finalize")
// 	{
// 		// Insert a call at the entry point of a routine to increment the call count
// 		RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)beforeFuncPrint, IARG_PTR, addr, IARG_END);
// 		RTN_InsertCall(rtn, IPOINT_AFTER, (AFUNPTR)afterFunc, IARG_PTR, addr, IARG_END);
		gblState.names.setupNewEntry(addr,RTN_Name(rtn));
// 	}

	RTN_Close(rtn);
}

VOID CallBack(VOID * ip, ADDRINT esp)
{
    UINT64 *RetAddrPtr = (UINT64 *)esp;
//     fprintf(log_info,"RET inst @%p ==> Retuen Address @%p.\n", ip, *RetAddrPtr);
	afterFunc((void*)*RetAddrPtr);
}

VOID Trace(TRACE trace, VOID *v)
{
//     ADDRINT insAddress = TRACE_Address(trace);

    // Visit every basic block in the trace
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
    {
        for(INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins))
        {   
            ADDRINT instAddress = INS_Address(ins);

            if( INS_IsCall(ins) )
            {
                ADDRINT nextInstAddress = (ADDRINT)( (USIZE)instAddress + INS_Size(ins) );
//                 fprintf(log_info,"CALL  inst @%p    ==> CALL Return Address @%p.\n", instAddress, nextInstAddress);
				INS_InsertCall( ins, 
                                IPOINT_BEFORE, 
                                (AFUNPTR)beforeFunc, 
                                IARG_PTR, 
                                nextInstAddress,
                                IARG_END);
            }  
            if(INS_IsRet(ins))
            {
                INS_InsertCall( ins, 
                                IPOINT_BEFORE, 
                                (AFUNPTR)CallBack, 
                                IARG_INST_PTR, 
                                IARG_REG_VALUE, 
                                REG_STACK_PTR, 
                                IARG_END);
            }
        }
    }
}

/**********************************************************/
//Inspirate from code exemple of pintool doc
static VOID instrImage(IMG img, VOID *v)
{
	instrImageMalloc(img);
	instrImageCalloc(img);
	instrImageFree(img);
}

/**********************************************************/
static VOID onExit(INT32 code, VOID *v)
{
	gblState.profiler.onExit();
// 	htopml::convertToJson(std::cout,gblState.names);
}

/**********************************************************/
static INT32 usage()
{
	cerr << "This tool produces a trace of calls to malloc." << endl;
	cerr << endl << KNOB_BASE::StringKnobSummary() << endl;
	return -1;
}

/**********************************************************/
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
	TRACE_AddInstrumentFunction(Trace,0);

	// Never returns
	PIN_StartProgram();

	return 0;
}
