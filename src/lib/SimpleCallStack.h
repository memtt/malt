#ifndef SIMPLE_BACKTRACE_H
#define SIMPLE_BACKTRACE_H

/********************  HEADERS  *********************/
#include <ostream>
#include <stdint.h>
#include "FuncNameDic.h"
#include <json/JsonState.h>

/*******************  FUNCTION  *********************/
typedef uint64_t SimpleBacktraceHash;

/*********************  CLASS  **********************/
class SimpleCallStack
{
	public:
		~SimpleCallStack(void);
		SimpleCallStack(void ** callStack,int size);
		SimpleBacktraceHash getSimpleHash(void) const;
		int getSize(void) const;
		void incrCnt(void);
		bool equal(void** callStack, int size) const;
		static SimpleBacktraceHash getSimpleHash(void ** calls,int size);
		void resolveSymbols(FuncNameDic & dic) const;
	public:
		friend std::ostream & operator << (std::ostream & out,const SimpleCallStack & tracer);
		friend void typeToJson(htopml::JsonState & json,std::ostream& stream, const SimpleCallStack & value);
	private:
		void ** calls;
		int size;
		int cnt;
};

#endif //SIMPLE_BACKTRACE_H
