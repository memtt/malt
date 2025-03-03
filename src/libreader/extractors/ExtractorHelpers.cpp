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
#include <iostream>
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

/**********************************************************/
void ExtractorHelpers::filterJson(nlohmann::json & value, const std::vector<std::string> & fieldsToKeep)
{
	std::vector<std::string> stack{"ROOT"};
	filterJson(value, fieldsToKeep, stack);
}

/**********************************************************/
void ExtractorHelpers::filterJson(nlohmann::json & value, const std::vector<std::string> & fieldsToKeep, std::vector<std::string> & stack)
{
	if (value.is_array()) {
		stack.push_back("[]");
		for (auto & it : value)
			filterJson(it, fieldsToKeep, stack);
		stack.pop_back();
	} else if (value.is_object()) {
		//search if need to remove somes
		std::vector<std::string> keysToRemove;
		for (const auto & it : value.items()) {
			stack.push_back(it.key());
			if (it.value().is_array() || it.value().is_object() ||  jsonCheckPath(fieldsToKeep, stack))
				filterJson(it.value(), fieldsToKeep, stack);
			else
				keysToRemove.push_back(it.key());
			stack.pop_back();
		}
		//remove
		for (const auto & it : keysToRemove)
			value.erase(it);
	}
}

/**********************************************************/
bool ExtractorHelpers::jsonCheckPath(const std::vector<std::string> & fieldsToKeep, const std::vector<std::string> & stack)
{
	std::string path;
	for (size_t i = 0 ; i < stack.size() ; i++) {
		if (path.empty())
			path = stack[stack.size() - i - 1];
		else
			path = stack[stack.size() - i - 1] + std::string(".") + path;
		for (const auto & accept : fieldsToKeep) {
			//std::cerr << accept << " == " << path << "==" << "*." + path << std::endl;
			if (accept == path || accept == "*." + path)
				return true;
		}
	}
	return false;
}

}
