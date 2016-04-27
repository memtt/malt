/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.3.0
             DATE     : 07/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <unistd.h>
#include <iostream>
#include "ProfileReader.hpp"

/**********************  USING  *********************/
using namespace MATT;

/*********************  CONSTS  *********************/
const char * test = "{ 'float' : 12.5 , \"emptyObject\" : { }, 'escape\\'':{}, 'string': 'value' , 'null': , 'string2': \"value\", 'bootTrue': true, 'boolFalse': false, 'array':[12, {}, true, false, 'text',,], 'child': {'value':19} }";

/*******************  FUNCTION  *********************/
int main(int argc, char ** argv)
{
	
	if (argc > 1)
	{
		ProfileReader reader(argv[1]);
		JsonNode & node = reader.getNode(argv[2]);
	} else {
		JsonReader reader;
		reader.loadString(test);
		JsonNode & node = reader.getRoot().getChild("");
		htopml::convertToJson(std::cout,node);
	}
	return EXIT_SUCCESS;
}
