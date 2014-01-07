/********************  HEADERS  *********************/
#include "CallStackInfo.h"

/*******************  FUNCTION  *********************/
CallStackInfo::CallStackInfo(void )
{
	this->cnt = 0;
}

/*******************  FUNCTION  *********************/
CallStackInfo::~CallStackInfo(void )
{

}

/*******************  FUNCTION  *********************/
void CallStackInfo::incrCnt(void )
{
	this->cnt++;
}

/*******************  FUNCTION  *********************/
void typeToJson(htopml::JsonState& json, std::ostream& stream, const CallStackInfo& value)
{
	json.openStruct();
	json.printField("cnt",value.cnt);
	json.closeStruct();
}

/*******************  FUNCTION  *********************/
std::ostream& operator<<(std::ostream& out, const CallStackInfo& info)
{
	out << "cnt = " << info.cnt;
	return out;
}
