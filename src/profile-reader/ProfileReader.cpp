/*****************************************************
             PROJECT  : MALT
             VERSION  : 0.3.0
             DATE     : 07/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//std
#include "ProfileReader.hpp"

/*******************  NAMESPACE  ********************/
namespace MALT
{

/*******************  FUNCTION  *********************/
ProfileReader::ProfileReader(const std::string& file)
{
	reader.load(file);
}

/*******************  FUNCTION  *********************/
ProfileReader::~ProfileReader(void)
{

}

}
