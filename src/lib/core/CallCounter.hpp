/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_CALL_COUNTER_HPP
#define MATT_CALL_COUNTER_HPP

/********************  HEADERS  *********************/
#include <cycle.h>


/*********************  TYPES  **********************/
namespace htopml
{
	class JsonState;
}

/********************  NAMESPACE  *******************/
namespace MATT
{

/*********************  CLASS  **********************/
class CallCounter
{
	public:
		CallCounter(void);
		void call(ticks t);
		friend void convertToJson(htopml::JsonState & json,const CallCounter & value);
		friend void convertToJson(htopml::JsonState & json,const class CallTimeSizeCounter & value);
		CallCounter & operator += (const CallCounter & obj);
	private:
		size_t count;
		ticks min;
		ticks max;
		ticks sum;
};

/*********************  CLASS  **********************/
class CallTimeSizeCounter : protected CallCounter
{
	public:
		CallTimeSizeCounter ( void );
		void call(ticks t,size_t s);
		friend void convertToJson(htopml::JsonState & json,const CallTimeSizeCounter & value);
	protected:
		size_t minSize;
		size_t maxSize;
		size_t sumSize;
};

}

#endif //MATT_CALL_COUNTER_HPP
