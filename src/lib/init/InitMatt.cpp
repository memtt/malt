/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include "InitMatt.hpp"
#include <hooks/EnterExitFunctionHooks.hpp>

/*******************  FUNCTION  *********************/
namespace MATT
{

/********************  GLOBALS  *********************/
static bool gblMattWasInit = false;
static InitMatt gblMatt;

/*******************  FUNCTION  *********************/
ThreadHooks * threadHookInit(void)
{
	if (!gblMattWasInit)
	{
		gblMatt.init();
		gblMattWasInit = true;
	}
	return &gblMatt;
}

/*******************  FUNCTION  *********************/
MallocHooks * mallocHookInit(void)
{
	if (!gblMattWasInit)
	{
		gblMatt.init();
		gblMattWasInit = true;
	}
	return &gblMatt;
}

/*******************  FUNCTION  *********************/
MmapHooks * mmapHookInit(void)
{
	if (!gblMattWasInit)
	{
		gblMatt.init();
		gblMattWasInit = true;
	}
	return &gblMatt;
}

/*******************  FUNCTION  *********************/
ExitHooks * exitHookInit(void)
{
	if (!gblMattWasInit)
	{
		gblMatt.init();
		gblMattWasInit = true;
	}
	return &gblMatt;
}

/*******************  FUNCTION  *********************/
EnterExitFunctionHooks * enterExitFunctionHooks(void)
{
	if (!gblMattWasInit)
	{
		gblMatt.init();
		gblMattWasInit = true;
	}
	return &gblMatt;
}

/*******************  FUNCTION  *********************/
InitMatt::~InitMatt ( void )
{

}

/*******************  FUNCTION  *********************/
void InitMatt::init ( void )
{

}

/*******************  FUNCTION  *********************/
bool InitMatt::mallocCallEnterExit ( void )
{
}

/*******************  FUNCTION  *********************/
bool InitMatt::mmapCallEnterExit ( void )
{
}

/*******************  FUNCTION  *********************/
void InitMatt::onAlignedAlloc ( MallocHooksInfos& info, void* ret, size_t alignment, size_t size )
{

}

/*******************  FUNCTION  *********************/
void InitMatt::onCalloc ( MallocHooksInfos& info, void* ret, size_t nmemb, size_t size )
{

}

/*******************  FUNCTION  *********************/
void InitMatt::onExit ( void )
{

}

/*******************  FUNCTION  *********************/
void InitMatt::onFree ( MallocHooksInfos& info, void* ptr )
{

}

/*******************  FUNCTION  *********************/
void InitMatt::onMalloc ( MallocHooksInfos& info, void* ret, size_t size )
{

}

/*******************  FUNCTION  *********************/
void InitMatt::onMallocEnterFunction ( MallocHooksInfos& info )
{

}

/*******************  FUNCTION  *********************/
void InitMatt::onMallocExitFunction ( MallocHooksInfos& info )
{

}

/*******************  FUNCTION  *********************/
void InitMatt::onMemalign ( MallocHooksInfos& info, void* ret, size_t alignment, size_t size )
{

}

/*******************  FUNCTION  *********************/
void InitMatt::onMmap ( MmapHooksInfos& info, void* res, void* start, size_t length, int prot, int flags, int fd, size_t offset )
{

}

/*******************  FUNCTION  *********************/
void InitMatt::onMmapEnterFunction ( MmapHooksInfos& info )
{

}

/*******************  FUNCTION  *********************/
void InitMatt::onMmapExitFunction ( MmapHooksInfos& info )
{

}

/*******************  FUNCTION  *********************/
void InitMatt::onMremap ( MmapHooksInfos& info, void* ret, void* old_address, size_t old_size, size_t new_size, int flags )
{

}

/*******************  FUNCTION  *********************/
void InitMatt::onMunmap ( MmapHooksInfos& info, int ret, void* start, size_t length )
{

}

/*******************  FUNCTION  *********************/
void InitMatt::onPosixMemalign ( MallocHooksInfos& info, int ret, void** memptr, size_t align, size_t size )
{

}

/*******************  FUNCTION  *********************/
void InitMatt::onPreFree ( MallocHooksInfos& info, void* ptr )
{

}

/*******************  FUNCTION  *********************/
void InitMatt::onPreRealloc ( MallocHooksInfos& info, void* ptr, size_t size )
{

}

/*******************  FUNCTION  *********************/
void InitMatt::onPvalloc ( MallocHooksInfos& info, void* ret, size_t size )
{

}

/*******************  FUNCTION  *********************/
void InitMatt::onRealloc ( MallocHooksInfos& info, void* ret, void* ptr, size_t size )
{

}

/*******************  FUNCTION  *********************/
void InitMatt::onThreadCreate ( void )
{

}

/*******************  FUNCTION  *********************/
void InitMatt::onThreadExit ( void )
{

}

/*******************  FUNCTION  *********************/
void InitMatt::onValloc ( MallocHooksInfos& info, void* ret, size_t size )
{

}

/*******************  FUNCTION  *********************/
bool InitMatt::isEnterExitFunction ( void )
{

}

/*******************  FUNCTION  *********************/
void InitMatt::onEnterFunction ( void* caller, void* function )
{

}

/*******************  FUNCTION  *********************/
void InitMatt::onExitFunction ( void* caller, void* function )
{

}

}
