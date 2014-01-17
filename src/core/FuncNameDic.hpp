#ifndef ATT_FUNC_NAME_DIC_HPP
#define ATT_FUNC_NAME_DIC_HPP

/********************  HEADERS  *********************/
#include <map>
#include <ostream>
// #include <json/JsonState.h>

/*******************  FUNCTION  *********************/
typedef std::map<void *,const char*> FuncNameDicMap;

/*********************  TYPES  **********************/
namespace htopml
{
	class JsonState;
};

/*******************  NAMESPACE  ********************/
namespace ATT
{

/*********************  CLASS  **********************/
class FuncNameDic
{
	public:
		FuncNameDic(void);
		~FuncNameDic(void);
		const char * getName(void * callSite);
		const char* setupNewEntry(void* callSite);
		const char * setupNewEntry(void * callSite,const std::string & name);
	public:
		friend std::ostream & operator << (std::ostream & out,const FuncNameDic & dic);
		friend void typeToJson(htopml::JsonState & json,std::ostream& stream, const FuncNameDic & value);
	private:
		FuncNameDicMap nameMap;
};

}

#endif //ATT_FUNC_NAME_DIC_HPP
