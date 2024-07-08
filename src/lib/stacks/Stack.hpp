/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 06/2023
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_STACK_H
#define MALT_STACK_H

/********************  HEADERS  *********************/
//std c
#include <stdint.h>
//std c++
#include <ostream>
//Include Debug
#include <common/Debug.hpp>

/*******************  FUNCTION  *********************/
/**
 * Define a hash of the stack.
**/
typedef uint64_t StackHash;

/*******************  FUNCTION  *********************/
namespace htopml
{
	class JsonState;
}

/*******************  NAMESPACE  ********************/
namespace MALT
{

/*********************  TYPES  **********************/
class SymbolSolver;

/*********************  ENUM  ***********************/
/**
 * Define the ordering of the stack storage. The ordering is forced by the backtrace implementation
 * and for the enter-exit mode, it is more efficient to use a reversed representation to enlarge the segment
 * quicly.
**/
enum StackOrder
{
	/** Use asc mode [child2 < child1 < main] for the backtrace mode. **/
	STACK_ORDER_ASC,
	/** Use desc mode [main > child1 > child2] for the enter-exit mode. **/
	STACK_ORDER_DESC
};

/**
 * To distinguish the type of the pointer in the Stack, either C address or a Python address.
 */
//TODO: Add ostream operator (<<)
//For C, keep the pointers as string
//For Python, prefix "PY-0x....."

enum DomainType {
	DOMAIN_C,
	DOMAIN_PYTHON,
};

struct AddressType {
	#if INTPTR_MAX == INT64_MAX /*64 Bits */

		/* 0 if it's a normal C address 1 if it's a Python address */
		int32_t domain:1;

		/* The address, on 63 bits */
		uintptr_t address:63;

	#elif INTPTR_MAX == INT32_MAX /* 32 Bits */
		int32_t domain;
		void* address;
	#else
		#error Unknown architecture
	#endif

	AddressType(DomainType domain=DOMAIN_C, void* address=nullptr) {
		this->domain = domain;
		this->address = (uintptr_t) address;
	}

	void set(DomainType domain, void* address){
		this->domain = domain;
		this->address = (uintptr_t) address;
	}

	void* getAddress() const {
		return (void*) this->address;
	}

	friend bool operator == (const AddressType & v1,const AddressType & v2){
		return ((v1.address == v2.address) && (v1.domain == v2.domain));
	}

	friend bool operator != (const AddressType & v1,const AddressType & v2){
		return ((v1.address != v2.address) || (v1.domain != v2.domain));
	}

	friend bool operator < (const AddressType & v1,const AddressType & v2){
		if (v1.domain < v2.domain){
			return true;
		}else if(v1.domain == v2.domain){
			return (v1.address < v2.address);
		}else{
			return false;
		}
	}

	friend bool operator >= (const AddressType & v1,const AddressType & v2){
		return ((v1.address >= v2.address) || (v1.domain >= v2.domain));
	}

	bool isNULL(){
		switch (domain){
			case DOMAIN_C:
				return (this->address == 0);	
			case DOMAIN_PYTHON:
				return false;
			default:
				MALT_FATAL_ARG("Undefined domain %1").arg(domain).end();
				return true;
		}
	}

	std::string toString() const;
};

extern const AddressType nullAddr;

std::ostream &operator<<(std::ostream &out, const AddressType &addrType);
void convertToJson(htopml::JsonState & json, const AddressType& addrType);

/*********************  CLASS  *************s*********/
/**
 * Provide a short reprensentation of a call stack as an array of instruction addresses.
 * This base class can be extended to build the backtrace stack representation of the enter-exit
 * one. It can also be used a a generic reprensentation to store the stack.
 * It allocate all the memory thought the internal allocator.
 * 
 * @brief Base class to store complete backtraces.
**/
class Stack
{
	public:
		Stack(StackOrder order);
		Stack(AddressType* stack,int size,StackOrder order);
		Stack(void** stack,int size,StackOrder order,DomainType domain/*=DOMAIN_C*/);
		Stack(const Stack & orig);
		Stack(const Stack & orig,int skipDepth);
		virtual ~Stack(void);
		StackHash hash(int skipDepth = 0) const;
		static StackHash hash(AddressType* stack, int size, MALT::StackOrder order);
		void solveSymbols(SymbolSolver & dic) const;
		void grow(void);
		bool isValid(void) const;
		int getSize(void) const;
		void set(AddressType* stack, int size,StackOrder order);
		void set (void** stack, int size, StackOrder order, DomainType domain);
		void set(const Stack & orig);
		AddressType getCaller(void) const;
		AddressType getCallee(void) const;
		AddressType operator[] (int idx) const;
		static bool partialCompare(const Stack & stack1,int skip1,const Stack & stack2,int skip2);
		Stack & operator = (const Stack & stack);
		size_t getMemSize(void) const;
		void fastSkip(int depth);
	public:
		friend std::ostream & operator << (std::ostream & out,const Stack & tracer);
		friend void convertToJson(htopml::JsonState & json, const Stack & value);
		friend bool operator == (const Stack & v1,const Stack & v2);
		friend bool operator < (const Stack & v1,const Stack & v2);
	protected:
		/** Pointer to the array of addresses to store the stack steps (due to fast skip it can be after the start of this->mem). **/
		//TODO: Pass Unique ID (search for the Unique ID map in miniMalt)
		//void ** stack;
		AddressType* stack;
		/** Pointer to the array of addresses to store the stack steps (ignoring fast skip). **/
		AddressType* mem;
		/** Size of the current stack. **/
		int size;
		/** Size of the buffer used to store the stack. **/
		int memSize;
	private:
		/** Keep track of the element ordering in the stack storage. **/
		StackOrder order;
};

//TODO: Make it in a single commit
//TODO: #ifdef 32 bits

/**
 * To distinguish the type of the pointer in the Stack, either C address or a Python address.
 */
struct AddressType{
	#if INTPTR_MAX == INT64_MAX /*64 Bits */

		/* 0 if it's a normal C address 1 if it's a Python address */
		int32_t domain:1;

		/* The address, on 63 bits */
		uint64_t address:63;

	#elif INTPTR_MAX == INT32_MAX /* 32 Bits */
		int32_t domain;
		void* address;
	#else
		#error Unknown architecture
	#endif
};

}
#endif //MALT_STACK_H
