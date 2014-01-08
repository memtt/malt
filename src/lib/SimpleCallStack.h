#ifndef SIMPLE_BACKTRACE_H
#define SIMPLE_BACKTRACE_H

/********************  HEADERS  *********************/
#include <ostream>
#include <stdint.h>
#include "FuncNameDic.h"
#include "CallStackInfo.h"
#include <json/JsonState.h>

/*******************  FUNCTION  *********************/
typedef uint64_t SimpleBacktraceHash;

/*********************  CLASS  **********************/
class SimpleCallStack
{
	public:
		SimpleCallStack(void);
		SimpleCallStack(const SimpleCallStack & orig);
		SimpleCallStack(void ** stack,int size);
		~SimpleCallStack(void);
		SimpleBacktraceHash getSimpleHash(void) const;
		int getSize(void) const;
		bool equal(void** stack, int size) const;
		static SimpleBacktraceHash getSimpleHash(void ** calls,int size);
		void resolveSymbols(FuncNameDic & dic) const;
		void loadCurrentStack(void);
		bool isValid(void) const;
		void set(void ** stack,int size);
	public:
		friend std::ostream & operator << (std::ostream & out,const SimpleCallStack & tracer);
		friend void typeToJson(htopml::JsonState & json,std::ostream& stream, const SimpleCallStack & value);
		friend bool operator == (const SimpleCallStack & v1,const SimpleCallStack & v2);
	private:
		void ** stack;
		int memSize;
		int size;
};

/*******************  FUNCTION  *********************/
inline int SimpleCallStack::getSize(void ) const
{
	return size;
}

/*******************  FUNCTION  *********************/
inline bool SimpleCallStack::isValid(void) const
{
	return (stack != NULL && size > 0);
}

#endif //SIMPLE_BACKTRACE_H
