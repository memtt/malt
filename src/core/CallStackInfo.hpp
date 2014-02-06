/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_CALL_STACK_INFO_HPP
#define MATT_CALL_STACK_INFO_HPP

/*******************  FUNCTION  *********************/
//standard
#include <ostream>
#include <cstdlib>
//from fftw (copied in extern-deps)
#include <cycle.h>

/*******************  FUNCTION  *********************/
namespace htopml
{
	class JsonState;
}

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*********************  CLASS  **********************/
struct SimpleQuantityHistory
{
	SimpleQuantityHistory(void);
	void addEvent(ssize_t value);
	void push(SimpleQuantityHistory & value);
	size_t count;
	ssize_t min;
	ssize_t max;
	ssize_t sum;
};

void convertToJson(htopml::JsonState& json, const SimpleQuantityHistory& value);

/*********************  CLASS  **********************/
struct CallStackInfo
{
// 	public:
		CallStackInfo(void);
		~CallStackInfo(void);
		void onAllocEvent(size_t value);
		void onFreeEvent(size_t value,ticks lifetime);
		void onFreeLinkedMemory(ssize_t value,ticks lifetime);
		void push(CallStackInfo & info);
		void writeAsCallgrindEntry(int line, std::ostream & out) const;
// 	public:
		
// 	private:
		SimpleQuantityHistory alloc;
		SimpleQuantityHistory free;
		SimpleQuantityHistory lifetime;
		ssize_t cntZeros;
		ssize_t alive;
		ssize_t maxAlive;
};

std::ostream & operator << (std::ostream & out,const CallStackInfo & info);
void convertToJson(htopml::JsonState& json, const CallStackInfo& value);

}

#endif //ATT_CALL_STACK_INFO_HPP
