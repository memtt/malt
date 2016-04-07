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
#include <cstring>

/*******************  NAMESPACE  ********************/
namespace MATT
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

/*******************  FUNCTION  *********************/
JsonNode & ProfileReader::getNode(const std::string& path)
{
	return reader.getRoot().getChild(path);
}

}
