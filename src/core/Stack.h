/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_STACK_H
#define MATT_STACK_H

/********************  HEADERS  *********************/
#include <ostream>
#include <stdint.h>
#include "FuncNameDic.hpp"
// #include <json/JsonState.h>

/*******************  FUNCTION  *********************/
typedef uint64_t StackHash;

/*******************  FUNCTION  *********************/
namespace htopml
{
	class JsonState;
};

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*********************  ENUM  ***********************/
enum StackOrder
{
	STACK_ORDER_ASC,
	STACK_ORDER_DESC,
};

/*********************  CLASS  **********************/
class Stack
{
	public:
		Stack(StackOrder order);
		Stack(void ** stack,int size,StackOrder order);
		Stack(const Stack & orig);
		virtual ~Stack(void);
		StackHash hash(void) const;
		static StackHash hash(void** stack, int size, MATT::StackOrder order);
		void resolveSymbols(FuncNameDic & dic) const;
		void grow(void);
		bool isValid(void) const;
		int getSize(void) const;
		void set(void ** stack, int size,StackOrder order);
		void set(const Stack & orig);
		void * getCaller(void) const;
		void * getCallee(void) const;
		void * operator[] (int idx);
	public:
		friend std::ostream & operator << (std::ostream & out,const Stack & tracer);
		friend void convertToJson(htopml::JsonState & json, const Stack & value);
		friend bool operator == (const Stack & v1,const Stack & v2);
	protected:
		void ** stack;
		int size;
		int memSize;
	private:
		StackOrder order;
};

}

#endif //MATT_STACK_H