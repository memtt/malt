#ifndef CALL_STACK_INFO_H
#define CALL_STACK_INFO_H

/*******************  FUNCTION  *********************/
#include <ostream>
#include <json/JsonState.h>

/*********************  CLASS  **********************/
class CallStackInfo
{
	public:
		CallStackInfo(void);
		~CallStackInfo(void);
		void incrCnt(void);
	public:
		friend std::ostream & operator << (std::ostream & out,const CallStackInfo & info);
		friend void typeToJson(htopml::JsonState& json, std::ostream& stream, const CallStackInfo& value);
	private:
		int cnt;
};

#endif //CALL_STACK_INFO_H
