/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_FUNC_NAME_DIC_HPP
#define MATT_FUNC_NAME_DIC_HPP

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
namespace MATT
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
		friend void convertToJson(htopml::JsonState & json, const FuncNameDic & value);
	private:
		FuncNameDicMap nameMap;
};

}

#endif //MATT_FUNC_NAME_DIC_HPP
