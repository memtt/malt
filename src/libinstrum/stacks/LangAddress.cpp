/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/stacks/Stack.cpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2024
*    AUTHOR   : Sébastien Valat (ECR) - 2014
***********************************************************/

/**********************************************************/
//htopml json converter
#include <json/JsonState.h>
//interne
#include <common/Debug.hpp>
#include "LangAddress.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
const LangAddress nullAddr = LangAddress(DOMAIN_C, 0);

/**********************************************************/
LangAddress::LangAddress(DomainType domain, void* address)
{
	this->domain = domain;
	this->address = (uintptr_t) address;
}

/**********************************************************/
bool LangAddress::operator == (const LangAddress & value) const
{
	return ((this->address == value.address) && (this->domain == value.domain));
}

/**********************************************************/
bool LangAddress::operator != (const LangAddress & value) const
{
	return ((this->address != value.address) || (this->domain != value.domain));
}

/**********************************************************/
bool LangAddress::operator < (const LangAddress & value) const
{
	if (this->domain < value.domain){
		return true;
	}else if(this->domain == value.domain){
		return (this->address < value.address);
	}else{
		return false;
	}
}

/**********************************************************/
bool LangAddress::operator >= (const LangAddress & value) const
{
	return ((this->address >= value.address) || (this->domain >= value.domain));
}

/**********************************************************/
LangAddress & LangAddress::operator-=(size_t delta)
{
	this->address--;
	return *this;
}

/**********************************************************/
bool LangAddress::isNULL(void) const{
	switch (this->domain){
		case DOMAIN_C:
		case DOMAIN_PYTHON:
		case DOMAIN_PYTHON_FRAME:
			return (this->address == 0);
		default:
			MALT_FATAL_ARG("Undefined domain %1").arg(domain).end();
			return true;
	}
}

/**********************************************************/
std::ostream &operator<<(std::ostream &out, const LangAddress &addrType){
	out << addrType.toString();
	
	return out;
}

/**********************************************************/
std::string LangAddress::toString() const {
	std::string value;

	if (this->domain == DOMAIN_PYTHON){
		value += "PY-";
	} else if (this->domain == DOMAIN_PYTHON_FRAME){
		value += "PYF-";
	} else if (this->domain == DOMAIN_SUB_STACK){
		value += "SS-";
	}

	char buffer[128];
	void * ptr = (void*)(uintptr_t)(this->address);
	snprintf(buffer, sizeof(buffer), "%p", ptr);
	value += buffer;

	return value;
}

/**********************************************************/
void convertToJson(htopml::JsonState & json, const LangAddress& addrType){
	json.printValue(addrType.toString());
}

}
