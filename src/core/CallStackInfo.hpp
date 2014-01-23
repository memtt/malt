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

void typeToJson(htopml::JsonState& json, std::ostream& stream, const SimpleQuantityHistory& value);

/*********************  CLASS  **********************/
struct CallStackInfo
{
// 	public:
		CallStackInfo(void);
		~CallStackInfo(void);
		void addEvent(ssize_t value, ticks lifetime);
		void onFreeLinkedMemory(ssize_t value,ticks lifetime);
		void push(CallStackInfo & info);
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
void typeToJson(htopml::JsonState& json, std::ostream& stream, const CallStackInfo& value);

}

#endif //ATT_CALL_STACK_INFO_HPP
