#ifndef ATT_CALL_STACK_INFO_HPP
#define ATT_CALL_STACK_INFO_HPP

/*******************  FUNCTION  *********************/
#include <ostream>
#include <cstdlib>
#include <cycle.h>

/*******************  FUNCTION  *********************/
namespace htopml
{
	class JsonState;
};

/*******************  NAMESPACE  ********************/
namespace ATT
{

/*********************  CLASS  **********************/
class SimpleQuantityHistory
{
	public:
		SimpleQuantityHistory(void);
		void addEvent(ssize_t value);
	public:
		friend void typeToJson(htopml::JsonState& json, std::ostream& stream, const SimpleQuantityHistory& value);
	private:
		size_t count;
		ssize_t min;
		ssize_t max;
		ssize_t sum;
};

/*********************  CLASS  **********************/
class CallStackInfo
{
	public:
		CallStackInfo(void);
		~CallStackInfo(void);
		void addEvent(ssize_t value, ticks lifetime);
		void onFreeLinkedMemory(ssize_t value,ticks lifetime);
	public:
		friend std::ostream & operator << (std::ostream & out,const CallStackInfo & info);
		friend void typeToJson(htopml::JsonState& json, std::ostream& stream, const CallStackInfo& value);
	private:
		SimpleQuantityHistory alloc;
		SimpleQuantityHistory free;
		SimpleQuantityHistory lifetime;
		ssize_t cntZeros;
		ssize_t alive;
		ssize_t maxAlive;
};

}

#endif //ATT_CALL_STACK_INFO_HPP
