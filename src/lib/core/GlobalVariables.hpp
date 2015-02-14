/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_GLOBAL_VARIABLES_HPP
#define MATT_GLOBAL_VARIABLES_HPP

/********************  HEADERS  *********************/
#include <map>
#include <tools/ELFReader.hpp>

/*********************  TYPES  **********************/
namespace htopml {
	class JsonState;
}

/*******************  NAMESPACE  ********************/
namespace MATT
{
	
/*********************  TYPES  **********************/
typedef std::map<std::string,ElfGlobalVariableVector> GlobalVariableMap;

/*********************  CLASS  **********************/
class GlobalVariables
{
	public:
		void load(void);
		friend void convertToJson(htopml::JsonState & json,const GlobalVariables & value);
	private:
		GlobalVariableMap globalVariables;
};

}

#endif //MATT_GLOBAL_VARIABLES_HPP
