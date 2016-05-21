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

/*******************  FUNCTION  *********************/
void ProfileReader::mapReduce(MapReduceHandler& handler, const std::string& path)
{
	JsonNode & tail = getNode(path);
	JsonNodeStack stack;
	MapReduceHandler * mapHandler = handler.allocate();
	stack.push(&tail);
	mapReduceInternal(handler,*mapHandler,reader.getRoot(),stack,tail);
	delete mapHandler;
}

/*******************  FUNCTION  *********************/
void ProfileReader::mapReduceInternal(MapReduceHandler& handler, MapReduceHandler& mapHandler, JsonNode& root, JsonNodeStack& stack, JsonNode& tail)
{
	stack.push(&tail);
	if (handler.filter(root,stack,tail))
	{
		mapHandler.map(root,stack,tail);
		handler.reduce(mapHandler);
	}
	for (auto & child : tail)
		mapReduceInternal(handler,mapHandler,root,stack,child);
	stack.pop();
}

}
