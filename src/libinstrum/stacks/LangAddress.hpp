/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.6
*    DATE     : 05/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/libinstrum/stacks/LangAddress.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

#ifndef MALT_LANG_ADDRESS_TYPE_H
#define MALT_LANG_ADDRESS_TYPE_H

/**********************************************************/
//std c
#include <stdint.h>
//std c++
#include <ostream>

/**********************************************************/
namespace htopml
{
	class JsonState;
}

/**********************************************************/
namespace MALT
{

/**********************************************************/
/**
 * To distinguish the type of the pointer in the Stack, either C address or a Python address.
 */
enum DomainType {
	DOMAIN_C,
	DOMAIN_PYTHON,
	DOMAIN_PYTHON_FRAME,
	DOMAIN_SUB_STACK,
};

/**********************************************************/
class LangAddress {
	public:
		LangAddress(DomainType domain=DOMAIN_C, void* address=nullptr);
		void set(DomainType domain, void* address) {this->domain = domain; this->address = (uintptr_t)address;};
		void setDomain(DomainType domain) {this->domain = domain;};
		void* getAddress(void) const {void * ptr = (void*)(size_t)(this->address); return ptr;};
		DomainType getDomain(void) const {return (DomainType)(this->domain);};
		bool isNULL(void) const;
		std::string toString(void) const;
	public:
		bool operator == (const LangAddress & value) const;
		bool operator != (const LangAddress & value) const;
		friend std::ostream &operator<<(std::ostream &out, const LangAddress &addrType);
		friend void convertToJson(htopml::JsonState & json, const LangAddress& addrType);
		bool operator < (const LangAddress & value) const;
		bool operator >= (const LangAddress & value) const;
		LangAddress & operator-=(size_t delta);
	private:
		#if INTPTR_MAX == INT64_MAX /*64 Bits */
			/* 0 if it's a normal C address 1 if it's a Python address */
			uint32_t domain:2;
			/* The address, on 63 bits */
			uintptr_t address:62;
		#elif INTPTR_MAX == INT32_MAX /* 32 Bits */
			uint32_t domain;
			void* address;
		#else
			#error Unknown architecture
		#endif
};

/**********************************************************/
extern const LangAddress nullAddr;

}

#endif //MALT_LANG_ADDRESS_TYPE_H
