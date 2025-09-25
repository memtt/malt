/***********************************************************
*    PROJECT  : MALT (MALoc Tracker)
*    DATE     : 07/2025
*    LICENSE  : CeCILL-C
*    FILE     : src/reader/libreader/json/File.hpp
*-----------------------------------------------------------
*    AUTHOR   : Sébastien Valat (INRIA) - 2025
***********************************************************/

/**********************************************************/
#ifndef MALT_JSON_FILE_HPP
#define MALT_JSON_FILE_HPP

/**********************************************************/
#include <string>
#include "Node.hpp"
#include "Parser.hpp"
#include "Json.hpp"

/**********************************************************/
namespace MALTJson
{

/**********************************************************/
class JsonFile
{
	public:
		JsonFile(const std::string & fname, bool progress = false);
		~JsonFile(void);
		Json getRoot(){return Json(&root);};
	private:
		void parse(void);
	private:
		std::string filename;
		JsonString data;
		JsonNode root;
		bool progress{false};
};

}

#endif //MALT_JSON_FILE_HPP
