/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    VERSION  : 1.2.4
*    DATE     : 10/2024
*    LICENSE  : CeCILL-C
*    FILE     : src/lib/stacks/Stack.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat - 2014 - 2024
*    AUTHOR   : Sébastien Valat (ECR) - 2014
***********************************************************/

#ifndef MALT_STACK_H
#define MALT_STACK_H

/**********************************************************/
//std c
#include <stdint.h>
//std c++
#include <ostream>
//Include Debug
#include <common/Debug.hpp>
#include <stacks/LangAddress.hpp>

/**********************************************************/
/**
 * Define a hash of the stack.
**/
typedef uint64_t StackHash;

/**********************************************************/
namespace htopml
{
	class JsonState;
}

/**********************************************************/
namespace MALT
{

/**********************************************************/
class SymbolSolver;

/**********************************************************/
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

/**********************************************************/
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
		Stack(LangAddress* stack,int size,StackOrder order);
		Stack(void** stack,int size,StackOrder order,DomainType domain/*=DOMAIN_C*/);
		Stack(const Stack & orig);
		Stack(const Stack & orig,int skipDepth);
		virtual ~Stack(void);
		StackHash hash(int skipDepth = 0) const;
		static StackHash hash(LangAddress* stack, int size, MALT::StackOrder order);
		void solveSymbols(SymbolSolver & dic) const;
		void grow(void);
		bool isValid(void) const;
		int getSize(void) const;
		void set(LangAddress* stack, int size,StackOrder order);
		void set (void** stack, int size, StackOrder order, DomainType domain);
		void set(const Stack & orig);
		LangAddress getCaller(void) const;
		LangAddress getCallee(void) const;
		LangAddress operator[] (int idx) const;
		LangAddress & operator[] (int idx);
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
		virtual void onGrow(size_t newSize);
	protected:
		/** Pointer to the array of addresses to store the stack steps (due to fast skip it can be after the start of this->mem). **/
		//TODO: Pass Unique ID (search for the Unique ID map in miniMalt)
		//void ** stack;
		LangAddress* stack;
		/** Pointer to the array of addresses to store the stack steps (ignoring fast skip). **/
		LangAddress* mem;
		/** Size of the current stack. **/
		int size;
		/** Size of the buffer used to store the stack. **/
		int memSize;
	private:
		/** Keep track of the element ordering in the stack storage. **/
		StackOrder order;
};

}

#endif //MALT_STACK_H
