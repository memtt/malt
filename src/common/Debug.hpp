/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef ATT_DEBUG_HPP
#define ATT_DEBUG_HPP

/********************  HEADERS  *********************/
#include "FormattedMessage.hpp"

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*********************  ENUM  ***********************/
enum DebugLevel
{
	MESSAGE_ASSERT,
	MESSAGE_DEBUG,
	MESSAGE_INFO,
	MESSAGE_NORMAL,
	MESSAGE_WARNING,
	MESSAGE_ERROR,
	MESSAGE_FATAL
};

/*********************  CLASS  **********************/
class Debug : public FormattedMessage
{
	public:
		Debug(const char * format,const char * file,int line,DebugLevel level = MESSAGE_DEBUG);
		Debug(const char * format,DebugLevel level = MESSAGE_DEBUG);
		virtual void end(void);
	protected:
		DebugLevel level;
		std::string file;
		std::string function;
		int line;
};

/********************  MACROS  **********************/
#define MATT_CODE_LOCATION __FILE__,__LINE__

/*******************  FUNCTION  *********************/
inline Debug debug(const char * format)   {return Debug(format,MESSAGE_DEBUG);  }
inline Debug warning(const char * format) {return Debug(format,MESSAGE_WARNING);}
inline Debug error(const char * format)   {return Debug(format,MESSAGE_ERROR);  }
inline Debug fatal(const char * format)   {return Debug(format,MESSAGE_FATAL);  }

/********************  MACROS  **********************/
#define MATT_FATAL(x)   MATT_FATAL_ARG(x).end()
#define MATT_DEBUG(x)   MATT_DEBUG_ARG(x).end()
#define MATT_ERROR(x)   MATT_ERROR_ARG(x).end()
#define MATT_WARNING(x) MATT_WARNING_ARG(x).end()
#define MATT_MESSAGE(x) MATT_MESSAGE_ARG(x).end()
#define MATT_INFO(x)    MATT_INFO_ARG(x).end()

/********************  MACROS  **********************/
#define MATT_FATAL_ARG(x)   MATT::Debug(x,MATT_CODE_LOCATION,MESSAGE_FATAL  )
#define MATT_DEBUG_ARG(x)   MATT::Debug(x,MATT_CODE_LOCATION,MESSAGE_DEBUG  )
#define MATT_ERROR_ARG(x)   MATT::Debug(x,MATT_CODE_LOCATION,MESSAGE_ERROR  )
#define MATT_WARNING_ARG(x) MATT::Debug(x,MATT_CODE_LOCATION,MESSAGE_WARNING)
#define MATT_MESSAGE_ARG(x) MATT::Debug(x,MATT_CODE_LOCATION,MESSAGE_NORMAL )
#define MATT_INFO_ARG(x)    MATT::Debug(x,MATT_CODE_LOCATION,MESSAGE_INFO )

/********************  MACROS  **********************/
#define assume(check,message) do { if (!(check)) MATT_FATAL(message); } while(0)
#define assumeArg(check,message) if (!(check)) MATT_FATAL_ARG(message)

/********************  MACROS  **********************/
#define MATT_TO_STRING(x) #x
#ifdef NDEBUG
	#define MATT_ASSERT(x)      do{} while(0)
#else
	#define MATT_ASSERT(x)      do{ if (!(x)) MATT::Debug(MATT_TO_STRING(x),MATT_CODE_LOCATION,MESSAGE_ASSERT).end(); } while(0)
#endif

}

#endif //ATT_DEBUG_HPP
