/*****************************************************
             PROJECT  : MALT
             VERSION  : 0.3.0
             DATE     : 07/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_PROFILE_READER_HPP
#define MALT_PROFILE_READER_HPP

/********************  HEADERS  *********************/
//std
#include <string>
//internal
#include "JsonReader.hpp"

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*********************  CLASS  **********************/
class ProfileReader
{
	public:
		ProfileReader(const std::string & file);
		~ProfileReader(void);
		JsonNode& getNode(const std::string& path);
	protected:
		JsonReader reader;
};

}

#endif //MALT_PROFILE_READER_HPP
