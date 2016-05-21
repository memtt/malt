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
#include <stack>
//internal
#include "JsonReader.hpp"

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*********************  TYPES  **********************/
typedef std::stack<JsonNode*> JsonNodeStack;

/*********************  CLASS  **********************/
class MapReduceHandler
{
	public:
		virtual bool filter(JsonNode & root,JsonNodeStack & stack,JsonNode & tail) = 0;
		virtual void map(JsonNode & root,JsonNodeStack & stack,JsonNode & tail) = 0;
		virtual void reduce(MapReduceHandler & handler) = 0;
		virtual MapReduceHandler * allocate(void) = 0;
};

/*********************  CLASS  **********************/
class ProfileReader
{
	public:
		ProfileReader(const std::string & file);
		~ProfileReader(void);
		JsonNode& getNode(const std::string& path);
		void mapReduce(MapReduceHandler & handler,const std::string & path);
	private:
		void mapReduceInternal(MapReduceHandler & handler, MapReduceHandler & mapHandler,JsonNode & root,JsonNodeStack & stack,JsonNode & tail);
	protected:
		JsonReader reader;
};

}

#endif //MALT_PROFILE_READER_HPP
