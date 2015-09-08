/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//standard
#include <cassert>
#include <cstdlib>
#include <iostream>
//header to implement
#include "Debug.hpp"

/*******************  NAMESPACE  ********************/
namespace MATT
{

/********************** CONSTS **********************/
/**
 * String to print names of debug levels.
**/
static const char * cstLevelPrefix[] = {"Assert : ","Debug : ","Info : ","","Warning : ","Error : ","Fatal : "};

/*******************  FUNCTION  *********************/
/**
 * Constructor of debug object. Arguments must be setup with arg() method and message printing
 * will be effective when calling the end() method.
 * @param format Define the message format (use %X for aguments where X is the ID of arg).
 * @param file Define the source location of the message emitter.
 * @param line Define the source location of the message emitter.
 * @param level Define the level of debug message.
 * 
@code
Debug msg("this is a test with arg %1 and arg %2 !").arg(10).arg("string").end();
@endcode
**/
Debug::Debug(const char* format, const char* file, int line, MATT::DebugLevel level)
	:FormattedMessage(format)
{
	this->file = file;
	this->line = line;
	this->level = level;
	this->emitted = false;
}

/*******************  FUNCTION  *********************/
/**
 * Short constructor without code location.
**/
Debug::Debug(const char* format, DebugLevel level)
	:FormattedMessage(format)
{
	this->file = "??";
	this->line = 0;
	this->emitted = false;
}

/*******************  FUNCTION  *********************/
/**
 * Emit the message.
**/
void Debug::end(void)
{
	this->emitted = true;
	switch(level)
	{
		case MESSAGE_DEBUG:
			#ifdef NDEBUG
				break;
			#endif //NDEBUG
		case MESSAGE_INFO:
		case MESSAGE_NORMAL:
			if (line != 0)
				std::cout << cstLevelPrefix[level] << "At " <<  file << ':' << line << " : \n";
			std::cout << cstLevelPrefix[level] << *this << std::endl;
			break;
		case MESSAGE_ASSERT:
			#ifdef NDEBUG
				break;
			#else
				if (line != 0)
					std::cout << cstLevelPrefix[level] << "At " <<  file << ':' << line << " : \n";
				std::cout << cstLevelPrefix[level] << *this << std::endl;
				abort();
			#endif
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

/*******************  FUNCTION  *********************/
/**
 * Destructor, only to check message emission on debug mode.
**/
Debug::~Debug(void)
{
	assert(emitted);
}

}
