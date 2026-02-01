/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 10/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/extractors/ExtractorHelpers.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#include <iostream>
#include <list>
#include <sstream>
#include <algorithm>
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
	"g_malloc0",
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
	"default_malloc",
	"default_free",
	"default_calloc",
	"default_realloc",
	"PyDataMem_UserNEW",
	"PyArray_NewFromDescr",
	"mmap",
	"munmap",
	"mremap",
	"__pgi_uacc_cuda_alloc",
	nullptr
};

/**********************************************************/
static const char * MALT_POWER_PS[] = {"&nbsp;","K","M","G","T","P"};
static const char * MALT_POWER_NPS[] = {" ","K","M","G","T","P"};
static const char * MALT_SUBPOWER_PS[] = {"&nbsp;","m","u","n"};
static const char * MALT_SUBPOWER_NPS[] = {" ","m","u","n","p"};

/**********************************************************/
bool ExtractorHelpers::isAllocFunction(const std::vector<std::string> & allocatorWrappers, const std::string & name)
{
	//check capture custom allocators
	if (std::find(allocatorWrappers.begin(), allocatorWrappers.end(), name) != allocatorWrappers.end())
		return true;

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
bool ExtractorHelpers::filterJson(nlohmann::json & value, const std::vector<std::string> & fieldsToKeep, std::vector<std::string> & stack)
{
	//keep all childs if selected
	if (jsonCheckPath(fieldsToKeep, stack))
		return false;

	//otherwise check
	bool removedSomes = false;
	bool removedAll = false;
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
			if (it.value().is_array() || it.value().is_object() ||  jsonCheckPath(fieldsToKeep, stack)) {
				if (filterJson(it.value(), fieldsToKeep, stack)) {
					keysToRemove.push_back(it.key());
					removedSomes = true;
				}
			} else {
				keysToRemove.push_back(it.key());
				removedSomes = true;
			}
			stack.pop_back();
		}
		//remove
		for (const auto & it : keysToRemove)
			value.erase(it);
		//all
		if (removedSomes && value.size() == 0)
			removedAll = true;
	}

	return removedAll;
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

/**********************************************************/
void ExtractorHelpers::jsonRemoveAbsPath(nlohmann::json & value, const std::string & prefix)
{
	if (value.is_string()) {
		std::string str = value;
		std::size_t pos = str.find(prefix);
		if (pos == std::string::npos) return;
		str.replace(pos, prefix.length(), "");
		value = str;
	} else if (value.is_array()) {
		for (auto & it : value)
			ExtractorHelpers::jsonRemoveAbsPath(it, prefix);
	} else if (value.is_object()) {
		for (auto & it : value.items())
			ExtractorHelpers::jsonRemoveAbsPath(it.value(), prefix);
	}
}

/**********************************************************/
bool ExtractorHelpers::jsonRemoveZeroes(nlohmann::json & value)
{
	bool removedAll = false;
	if (value.is_number()) {
		size_t v = value;
		if (v == 0)
			return true;
	} else if (value.is_array()) {
		std::list<size_t> keysToRemove;
		size_t i = 0;
		for (auto & it : value) {
			if (ExtractorHelpers::jsonRemoveZeroes(it))
				keysToRemove.push_back(i);
			i++;
		}
		if (keysToRemove.size() == value.size()) {
			removedAll = true;
		} else {
			size_t cnt = 0;
			for (auto & it : keysToRemove)
				value.erase(it - (cnt++));
		}
	} else if (value.is_object()) {
		std::list<std::string> keysToRemove;
		for (auto & it : value.items()) {
			if (ExtractorHelpers::jsonRemoveZeroes(it.value()))
				keysToRemove.push_back(it.key());
		}
		if (keysToRemove.size() == value.size()) {
			removedAll = true;
		} else {
			for (auto & it : keysToRemove)
				value.erase(it);
		}
	}

	//ok not removed all
	return removedAll;
}

/**********************************************************/
nlohmann::json ExtractorHelpers::buildShorterFlatProfileSummary(nlohmann::json value, bool onlyLocation)
{
	//reshape
	nlohmann::json result = nlohmann::json::object();

	//loop
	for (auto & it : value) {
		std::stringstream key;
		std::string file = it["location"]["file"];
		std::string function = it["location"]["function"];
		key << file << ":" << function << ":" << it["location"]["line"];
		result[key.str()] = it["own"];
	}

	//remove zeroes
	ExtractorHelpers::jsonRemoveZeroes(result);

	//case
	if (onlyLocation) {
		for (auto & it : result.items()) {
			it.value() = true;
		}
	}

	//ok
	return result;
}

/********************************************************************/
/** Short helper to convert values to human readable format **/	
std::string ExtractorHelpers::humanReadable(double value,int decimals,const std::string & unit, bool protectedSpace)
{
	//select kilo / mega base
	size_t mul = 1000;
	if (unit == "B" || unit == "B")
		mul = 1024;
	
	//fix decimals
	if (value >= 1 && value < mul)
		decimals = 0;
	if (value == 0)
		decimals = 0;

	//out
	std::stringstream buffer;
	buffer.precision(decimals);
	
	//handle kilo vs mili
	if (value >= 0.1 || value == 0)
	{
		int power = 0;
		while (value >= mul)
		{
			power++;
			value /= mul;
		}

		if (protectedSpace)
			buffer << std::fixed << value << " " << MALT_POWER_PS[power] << unit;
		else
			buffer << std::fixed <<  value << " " << MALT_POWER_NPS[power] << unit;
	} else {
		int power = 0;
		while (value < 0.1 && power < 4)
		{
			power++;
			value *= 1000.0;
		}

		if (protectedSpace)
			buffer << std::fixed << value << " " << MALT_SUBPOWER_PS[power] << unit;
		else
			buffer << std::fixed << value << " " << MALT_SUBPOWER_NPS[power] << unit;
	}

	//ok
	return buffer.str();
}

}
