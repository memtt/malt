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
#include "AddressType.hpp"

/**********************************************************/
namespace MALT
{

/**********************************************************/
const AddressType nullAddr = {DOMAIN_C, 0};

/**********************************************************/
AddressType::AddressType(DomainType domain, void* address)
{
	this->domain = domain;
	this->address = (uintptr_t) address;
}

/**********************************************************/
bool AddressType::operator == (const AddressType & value) const
{
	return ((this->address == value.address) && (this->domain == value.domain));
}

/**********************************************************/
bool AddressType::operator != (const AddressType & value) const
{
	return ((this->address != value.address) || (this->domain != value.domain));
}

/**********************************************************/
bool AddressType::operator < (const AddressType & value) const
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
bool AddressType::operator >= (const AddressType & value) const
{
	return ((this->address >= value.address) || (this->domain >= value.domain));
}

/**********************************************************/
AddressType & AddressType::operator-=(size_t delta)
{
	this->address--;
	return *this;
}

/**********************************************************/
bool AddressType::isNULL(void) const{
	switch (this->domain){
		case DOMAIN_C:
			return (this->address == 0);	
		case DOMAIN_PYTHON:
			return false;
		default:
			MALT_FATAL_ARG("Undefined domain %1").arg(domain).end();
			return true;
	}
}

/**********************************************************/
std::ostream &operator<<(std::ostream &out, const AddressType &addrType){
	out << addrType.toString();
	
	return out;
}

/**********************************************************/
std::string AddressType::toString() const {
	std::string value;

	if (this->domain == DOMAIN_PYTHON){
		value += "PY-";
	}

	char buffer[64];
	void * ptr = (void*) this->address;
	sprintf(buffer, "%p", ptr);
	value += buffer;

	return value;
}

/**********************************************************/
void convertToJson(htopml::JsonState & json, const AddressType& addrType){
	json.printValue(addrType.toString());
}

}
