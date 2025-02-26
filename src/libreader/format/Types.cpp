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
#include "Types.hpp"

/**********************************************************/
namespace MALTFormat
{

/**********************************************************/
void to_json(nlohmann::json & json, const LangAddress & value)
{
	//select lang mode
	std::string lang;
	switch(value.lang) {
		case LANG_PYTHON:
			lang = "PY-";
			break;
		case LANG_C:
			break;
	}

	//convert
	char buffer[512];
	if (value.address == nullptr)
		snprintf(buffer, sizeof(buffer), "%s0x0", lang.c_str());
	else
		snprintf(buffer, sizeof(buffer), "%s%p", lang.c_str(), value.address);

	//set
	json = buffer;
}

/**********************************************************/
void from_json(const nlohmann::json & json, LangAddress & address)
{
	//check
	assert(json.is_string());

	//parse
	std::string value = json;

	//cases
	void * ptr = nullptr;
	if (sscanf(value.c_str(), "PY-%p", &ptr) == 1) {
		address.lang = LANG_PYTHON;
		address.address = ptr;
	} else if (sscanf(value.c_str(), "%p", &ptr) == 1) {
		address.lang = LANG_C;
		address.address = ptr;
	} else {
		assert(false);
	}
}


/**********************************************************/
void from_json(const nlohmann::json & json, void * & ptr)
{
	//check
	assert(json.is_string());

	//convert
	std::string value = json;

	//parse
	if (sscanf(value.c_str(), "%p", &ptr) == 0)
		assert(false);
}

/**********************************************************/
void to_json(nlohmann::json & json, const void *& ptr)
{
	char buffer[256];
	snprintf(buffer, sizeof(buffer), "%p", ptr);
	json = buffer;
}

}
