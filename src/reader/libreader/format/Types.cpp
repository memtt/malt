/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 07/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/format/Types.cpp
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
	//set
	json = to_string(value);
}

/**********************************************************/
void from_json(const JsonIn & json, LangAddress & address)
{
	//check
	assert(json.is_string());

	//parse
	std::string value;
	json.get_to(value);

	//convert
	address = to_lang_address(value);
}

/**********************************************************/
std::string to_string(const LangAddress & value)
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
	return buffer;
}

/**********************************************************/
LangAddress to_lang_address(const std::string & value)
{
	//cases
	void * ptr = nullptr;
	LangAddress address{LANG_C, 0};
	if (sscanf(value.c_str(), "PY-%p", &ptr) == 1) {
		address.lang = LANG_PYTHON;
		address.address = ptr;
	} else if (sscanf(value.c_str(), "%p", &ptr) == 1) {
		address.lang = LANG_C;
		address.address = ptr;
	} else {
		assert(false);
	}
	return address;
}

/**********************************************************/
void from_json(const JsonIn & json, void * & ptr)
{
	//check
	assert(json.is_string());

	//convert
	std::string value;
	json.get_to(value);

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

/**********************************************************/
bool operator<(const LangAddress & a, const LangAddress & b)
{
	if (a.lang < b.lang)
		return true;
	else if (a.lang == b.lang && a.address < b.address)
		return true;
	else
		return false;
}

/**********************************************************/
bool operator==(const LangAddress & a, const LangAddress & b)
{
	return a.lang == b.lang && a.address == b.address;
}

}
