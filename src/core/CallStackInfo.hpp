#ifndef ATT_CALL_STACK_INFO_HPP
#define ATT_CALL_STACK_INFO_HPP

/*******************  FUNCTION  *********************/
#include <ostream>
#include <cstdlib>

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
		void addEvent(ssize_t value);
	public:
		friend std::ostream & operator << (std::ostream & out,const CallStackInfo & info);
		friend void typeToJson(htopml::JsonState& json, std::ostream& stream, const CallStackInfo& value);
	private:
		SimpleQuantityHistory alloc;
		SimpleQuantityHistory free;
		ssize_t cntZeros;
};

}

#endif //ATT_CALL_STACK_INFO_HPP