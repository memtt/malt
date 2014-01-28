/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <cstdlib>
#include <iostream>
#include "Debug.hpp"

/*******************  NAMESPACE  ********************/
namespace MATT
{

/********************** CONSTS **********************/
static const char * cstLevelPrefix[] = {"Debug : ","","Warning : ","Error : ","Fatal : "};

/*******************  FUNCTION  *********************/
Debug::Debug(const char* format, const char* file, int line, MATT::DebugLevel level)
	:FormattedMessage(format)
{
	this->file = file;
	this->line = line;
	this->level = level;
}

/*******************  FUNCTION  *********************/
Debug::Debug(const char* format, DebugLevel level)
	:FormattedMessage(format)
{
	this->file = "??";
	this->line = 0;
}

/*******************  FUNCTION  *********************/
void Debug::end(void)
{
	switch(level)
	{
		case MESSAGE_INFO:
		case MESSAGE_NORMAL:
			if (line != 0)
				std::cout << cstLevelPrefix[level] << "At " <<  file << ':' << line << " : \n";
			std::cout << cstLevelPrefix[level] << *this << std::endl;
			break;
		case MESSAGE_ASSERT:
		case MESSAGE_DEBUG:
			#ifdef NDEBUG
			break;
			#endif //NDEBUG
		case MESSAGE_WARNING:
		case MESSAGE_ERROR:
			if (line != 0)
				std::cerr << cstLevelPrefix[level] << "At " <<  file << ':' << line << " : \n";
			std::cerr << cstLevelPrefix[level] << *this << std::endl;
			break;
		case MESSAGE_FATAL:
			if (line != 0)
				std::cerr << cstLevelPrefix[level] << "At " <<  file << ':' << line << " : \n";
			std::cerr << cstLevelPrefix[level] << *this << std::endl;
			abort();
			break;
	}
}

};
