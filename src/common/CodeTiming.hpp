/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_CODE_TIMING_H
#define MATT_CODE_TIMING_H

/********************  HEADERS  *********************/
//standards
#include <string>
#include <cstdlib>
//from fftw (copied into extern-deps)
#include <cycle.h>

/********************  MACROS  **********************/
/**
 * A short macro to quicly profile some code lines. 
\code{.cpp}
void yourFunc(void)
{
	CODE_TIMING( "rand" , int r = rand() );
}
\endcode
**/
#define CODE_TIMING(name,code) do{code;}while(0)
// #define CODE_TIMING(name,code) \
// 	do {  \
// 		static MATT::CodeTiming __code_timing_local__(name);   \
// 		__code_timing_local__.start(); \
// 		do {code;} while(0); \
// 		__code_timing_local__.end();  \
// 	} while(0)

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*********************  CLASS  **********************/
/**
 * @brief Short code to profile functions.
 * 
 * Provide some basic mecanism to help optimization by measuring execution time of some codes.
 * Objects of this class may be declared static of globals to get the same lifecycle than the process.
 * The basic usage might be :
\code{.cpp}
void yourFun(void)
{
	static CodeTiming timing( "youFunc" );
	timing.start();
	//your job
	timing.stop();
}
\endcode
 *
 * \b CAUTION, it didn't support recursion.
**/
class CodeTiming
{
	public:
		CodeTiming(const char * name);
		~CodeTiming(void);
		void start(void);
		void end(void);
	private:
		void printValue(double value, const char * unit = "");
	private:
		/** Name of the code section to profile, only used for display. **/
		std::string name;
		/** Cumulativ execution time spent in the profiled code. **/
		ticks sum;
		/** Minimum execution time of the profiled code. **/
		ticks min;
		/** Maximum execution time of the profiled code. **/
		ticks max;
		/** Number of calls. **/
		size_t count;
		/** Remember the starting time of current measurement. **/
		ticks lastStart;
		/** Remember the time at initialization of this object. **/
		ticks globalStart;
};

/*******************  FUNCTION  *********************/
/** Start measurement of a new call to your code. **/
inline void CodeTiming::start(void)
{
	this->lastStart = getticks();
}

/*******************  FUNCTION  *********************/
/** End measurement of the current call to your code. **/
inline void CodeTiming::end(void)
{
	//get time
	ticks t =getticks() - lastStart;
	
	//update min/max
	if (count == 0)
	{
		min = max = t;
	} else {
		if (t < min) min = t;
		if (t > max) max = t;
	}
	
	//update global sums
	count++;
	sum += t;
}

/*******************  FUNCTION  *********************/
ticks ticksPerSecond(void);

};

#endif //MATT_CODE_TIMING_H
