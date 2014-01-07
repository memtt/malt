#ifndef FUNC_NAME_DIC_H
#define FUNC_NAME_DIC_H

/********************  HEADERS  *********************/
#include <map>
#include <ostream>
#include <json/JsonState.h>

/*******************  FUNCTION  *********************/
typedef std::map<void *,const char*> FuncNameDicMap;

/*********************  CLASS  **********************/
class FuncNameDic
{
	public:
		FuncNameDic(void);
		~FuncNameDic(void);
		const char * getName(void * callSite);
		const char* setupNewEntry(void* callSite);
	public:
		friend std::ostream & operator << (std::ostream & out,const FuncNameDic & dic);
		friend void typeToJson(htopml::JsonState & json,std::ostream& stream, const FuncNameDic & value);
	private:
		FuncNameDicMap nameMap;
};

#endif //FUNC_NAME_DIC_H
