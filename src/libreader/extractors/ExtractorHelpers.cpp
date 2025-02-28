/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/tools/Backtrace.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include "ExtractorHelpers.hpp"

/**********************************************************/
namespace MALTReader
{

/**********************************************************/
//List of allocator functions
static const char * CST_ALLOC_FUNC_NAMES[] = {
	"gomp_realloc",
	"gomp_malloc",
	"gomp_free",
	"__gnu_cxx::new_allocator",
	"operator new",
	"operator delete",
	"_Zna",
	"_Znw",
	"g_malloc",
	"g_realloc",
	"g_free",
	"for__get_vm",
	"for__free_vm",
	"malloc",
	"calloc",
	"free",
	"realloc",
	"memalign",
	"posix_memalign",
	"for_alloc_allocatable",
	"for_dealloc_allocatable",
	"for_allocate",
	"for_deallocate",
	"MALT::WrapperPython",
	nullptr
};

/**********************************************************/
bool ExtractorHelpers::isAllocFunction(const std::string & name)
{
	//search all
	const char ** cursor = CST_ALLOC_FUNC_NAMES;
	while (*cursor != nullptr) {
		if (name.rfind(*cursor,0 ) == 0)
			return true;
		cursor++;
	}
	return false;
}

}
