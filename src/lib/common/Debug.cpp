/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.2.2
             DATE     : 06/2023
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//standard
#include <cassert>
#include <cstdlib>
#include <iostream>
//internal
#include "Options.hpp"
//header to implement
#include "Debug.hpp"

/*******************  NAMESPACE  ********************/
namespace MALT
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
Debug::Debug(const char* format, const char* file, int line, MALT::DebugLevel level)
	:FormattedMessage(format)
{
	this->file = file;
	this->line = line;
	this->level = level;
	this->emitted = false;
	this->fakeAbort = false;
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
	this->level = level;
}

/*******************  FUNCTION  *********************/
void Debug::abort(void)
{
	if (this->fakeAbort)
		this->fakeAbort = false;
	else
		::abort();
}

/*******************  FUNCTION  *********************/
Debug & Debug::enableFakeAbort(void)
{
	this->fakeAbort = true;
	return *this;
}

/*******************  FUNCTION  *********************/
bool Debug::aborted(void) const
{
	return this->fakeAbort == false;
}

/*******************  FUNCTION  *********************/
void Debug::end(void)
{
	this->end(std::cout, std::cerr);
}

/*******************  FUNCTION  *********************/
/**
 * Emit the message.
**/
void Debug::end(std::ostream & out, std::ostream & err)
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
				out << std::endl << cstLevelPrefix[level] << "At " <<  file << ':' << line << " : \n";
			out << cstLevelPrefix[level] << *this << std::endl;
			break;
		case MESSAGE_ASSERT:
			#ifdef NDEBUG
				break;
			#else
				if (line != 0)
					err << std::endl << cstLevelPrefix[level] << "At " <<  file << ':' << line << " : \n";
				err << cstLevelPrefix[level] << *this << std::endl;
				this->abort();
				break;
			#endif
		case MESSAGE_WARNING:
			if (gblOptions != NULL)
				if (gblOptions->outputVerbosity <= MALT_VERBOSITY_DEFAULT)
					break;
		case MESSAGE_ERROR:
			if (line != 0)
				err << std::endl << cstLevelPrefix[level] << "At " <<  file << ':' << line << " : \n";
			err << cstLevelPrefix[level] << *this << std::endl;
			break;
		case MESSAGE_FATAL:
			if (line != 0)
				err << std::endl << cstLevelPrefix[level] << "At " <<  file << ':' << line << " : \n";
			err << cstLevelPrefix[level] << *this << std::endl;
			this->abort();
			break;
	}
}

/*******************  FUNCTION  *********************/
/**
 * Destructor, only to check message emission on debug mode.
**/
Debug::~Debug(void)
{
	if (!emitted)
		this->end();
}

}
