/*****************************************************
             PROJECT  : MALT
             VERSION  : 0.3.0
             DATE     : 07/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <unistd.h>
#include "JsonReader.hpp"

/**********************  USING  *********************/
using namespace MALT;

/*********************  CONSTS  *********************/
const char * test = "{ 'float' : 12.5 , \"emptyObject\" : { }, 'escape\\'':{}, 'string': 'value' , 'null': , 'string2': \"value\", 'bootTrue': true, 'boolFalse': false, 'array':[12, {}, true, false, 'text',,] }";

/*******************  FUNCTION  *********************/
int main(int argc, char ** argv)
{
	JsonReader reader;
	
	if (argc > 1)
		reader.load(argv[1]);
	else
		reader.loadString(test);
	return EXIT_SUCCESS;
}
